#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <optional>

using namespace std;
using namespace sf;

const int   ROWS      = 52;
const int   COLS      = 52;
const float tileW     = 50.f;
const float tileH     = 25.f;
const float originX   = 560.f;
const float originY   = -300.f;
const int   SHEET_COLS = 11;
const int   SHEET_ROWS = 11;
const int   TILE_PX    = 32;

int textureMap  [ROWS][COLS];
int elevationMap[ROWS][COLS];
int directionMap[ROWS][COLS];

// ─────────────────────────────────────────────────────────────────────────────
Vector2f gridToScreen(int row, int col)
{
    return {
        originX + (col - row) * (tileW / 2.f),
        originY + (col + row) * (tileH / 2.f)
    };
}

pair<int,int> screenToTile(float sx, float sy)
{
    float dx = sx - (originX + tileW / 2.f);
    float dy = sy - originY;
    float fcol = (dx / (tileW/2.f) + dy / (tileH/2.f)) / 2.f;
    float frow = (dy / (tileH/2.f) - dx / (tileW/2.f)) / 2.f;
    int baseRow = (int)floor(frow), baseCol = (int)floor(fcol);
    int bestRow = baseRow, bestCol = baseCol;
    float bestDist = __FLT_MAX__;
    for (int dr = 0; dr <= 1; dr++)
        for (int dc = 0; dc <= 1; dc++) {
            int r = baseRow+dr, c = baseCol+dc;
            float cx = (originX+tileW/2.f) + (c-r)*(tileW/2.f);
            float cy = originY + (c+r)*(tileH/2.f) + tileH/2.f;
            float d  = (sx-cx)*(sx-cx) + (sy-cy)*(sy-cy);
            if (d < bestDist) { bestDist=d; bestRow=r; bestCol=c; }
        }
    return {bestRow, bestCol};
}

IntRect indexToRect(int idx)
{
    if (idx < 0) return {{0,0},{0,0}};
    return {{ (idx%SHEET_COLS)*TILE_PX, (idx/SHEET_COLS)*TILE_PX }, {TILE_PX,TILE_PX}};
}

// ── Save ─────────────────────────────────────────────────────────────────────
void saveLayout()
{
    ofstream f("layout.txt");
    f << "# Texture map (" << ROWS << "x" << COLS << ")\n";
    for (int i=0;i<ROWS;i++) {
        for (int j=0;j<COLS;j++) f << textureMap[i][j] << (j<COLS-1?" ":"");
        f << "\n";
    }
    f << "\n# Elevation map (" << ROWS << "x" << COLS << ")\n";
    for (int i=0;i<ROWS;i++) {
        for (int j=0;j<COLS;j++) f << elevationMap[i][j] << (j<COLS-1?" ":"");
        f << "\n";
    }
    f << "\n# Direction map (" << ROWS << "x" << COLS << ") 0=none 1=down 2=left 3=right 4=up\n";
    for (int i=0;i<ROWS;i++) {
        for (int j=0;j<COLS;j++) f << directionMap[i][j] << (j<COLS-1?" ":"");
        f << "\n";
    }
    cerr << "Saved layout.txt\n";
}

// ── Load ─────────────────────────────────────────────────────────────────────
void loadLayout()
{
    ifstream f("layout.txt");
    if (!f) { cerr << "No layout.txt found — starting fresh\n"; return; }

    string line;
    auto readMap = [&](int map[][COLS]) {
        int i = 0;
        while (i < ROWS && getline(f, line)) {
            if (line.empty() || line[0]=='#') continue;
            istringstream ss(line);
            for (int j=0;j<COLS;j++) ss >> map[i][j];
            i++;
        }
    };

    readMap(textureMap);
    readMap(elevationMap);
    readMap(directionMap);
    cerr << "Loaded layout.txt\n";
}

// ─────────────────────────────────────────────────────────────────────────────
int main()
{
    // init
    for (int i=0;i<ROWS;i++)
        for (int j=0;j<COLS;j++)
        { textureMap[i][j]=-1; elevationMap[i][j]=0; directionMap[i][j]=0; }

    loadLayout();

    ContextSettings settings;
    settings.antiAliasingLevel = 8;
    RenderWindow window(VideoMode({1200,700}), "Grid Editor  |  Enter=arm texture  F=arm direction  Up/Down=elevation  R=reset  Ctrl+S=save",
                        Style::Default, State::Windowed, settings);
    window.setFramerateLimit(60);

    Texture sheet("./Assets/isometric/spritesheet.png");

    // ── Tile shapes ──────────────────────────────────────────────────────────
    struct Tile { ConvexShape shape; int row, col; };
    vector<Tile> tiles;
    tiles.reserve(ROWS*COLS);

    for (int i=0;i<ROWS;i++)
        for (int j=0;j<COLS;j++) {
            Tile t; t.row=i; t.col=j;
            t.shape.setPointCount(6);
            t.shape.setPoint(0, {tileW/2.f, 0.f});
            t.shape.setPoint(1, {tileW,     tileH});
            t.shape.setPoint(2, {tileW,     tileH+0.4f*tileH});
            t.shape.setPoint(3, {tileW/2.f, tileH*2.f});
            t.shape.setPoint(4, {0.f,       tileH+0.4f*tileH});
            t.shape.setPoint(5, {0.f,       tileH});
            t.shape.setFillColor(Color(50,50,60,200));
            t.shape.setOutlineColor(Color(90,90,90,150));
            t.shape.setOutlineThickness(0.5f);
            Vector2f pos = gridToScreen(i,j);
            pos.y -= elevationMap[i][j];
            t.shape.setPosition(pos);
            tiles.push_back(t);
        }

    // ── Font ─────────────────────────────────────────────────────────────────
    Font font;
    bool hasFont = false;
    for (auto& p : {
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
    }) { if (font.openFromFile(p)) { hasFont=true; break; } }

    // ── HUD ──────────────────────────────────────────────────────────────────
    RectangleShape hudBg({1200.f, 44.f});
    hudBg.setFillColor(Color(10,10,20,230));
    hudBg.setPosition({0.f,656.f});

    Text topLine(font);   // mode/armed status
    topLine.setCharacterSize(13);
    topLine.setFillColor(Color::Cyan);
    topLine.setPosition({8.f,658.f});

    Text botLine(font);   // last tile info
    botLine.setCharacterSize(12);
    botLine.setFillColor(Color(180,180,180));
    botLine.setPosition({8.f,675.f});

    // ── Editor state ─────────────────────────────────────────────────────────
    string inputBuffer  = "";

    int  activeTexIdx   = -1;
    bool texReady       = false;

    int  activeDir      = 0;
    bool dirReady       = false;

    int  lastRow        = -1;
    int  lastCol        = -1;

    // ── View ─────────────────────────────────────────────────────────────────
    View gameView(FloatRect({0.f,0.f},{1200.f,700.f}));
    bool     panning = false;
    Vector2i panStart;
    Vector2f viewCenterAtPan;

    // helper — find tile in vector
    auto findTile = [&](int r, int c) -> Tile* {
        for (auto& t : tiles) if (t.row==r && t.col==c) return &t;
        return nullptr;
    };

    // ─────────────────────────────────────────────────────────────────────────
    while (window.isOpen())
    {
        while (const optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>()) window.close();

            // ── Keyboard ─────────────────────────────────────────────────────
            if (const auto* kp = event->getIf<Event::KeyPressed>())
            {
                // Ctrl+S
                if (kp->control && kp->code == Keyboard::Key::S)
                    saveLayout();

                // Backspace
                if (kp->code == Keyboard::Key::Backspace && !inputBuffer.empty())
                    inputBuffer.pop_back();

                // Escape — cancel everything
                if (kp->code == Keyboard::Key::Escape) {
                    inputBuffer = "";
                    texReady = false; activeTexIdx = -1;
                    dirReady = false; activeDir    =  0;
                }

                // Enter — arm as TEXTURE
                if (kp->code == Keyboard::Key::Enter && !inputBuffer.empty()) {
                    try {
                        int idx = stoi(inputBuffer);
                        if (idx >= 0 && idx < SHEET_ROWS*SHEET_COLS) {
                            activeTexIdx = idx;
                            texReady     = true;
                            dirReady     = false; // one mode at a time
                        }
                    } catch(...) {}
                    inputBuffer = "";
                }

                // F — arm as DIRECTION
                if (kp->code == Keyboard::Key::F && !inputBuffer.empty()) {
                    try {
                        int d = stoi(inputBuffer);
                        if (d >= 0 && d <= 4) {
                            activeDir = d;
                            dirReady  = true;
                            texReady  = false; // one mode at a time
                        }
                    } catch(...) {}
                    inputBuffer = "";
                }

                // Up/Down — elevation on last tile
                if (lastRow >= 0 &&
                    (kp->code == Keyboard::Key::Up || kp->code == Keyboard::Key::Down))
                {
                    int delta = (kp->code == Keyboard::Key::Up) ? 1 : -1;
                    elevationMap[lastRow][lastCol] += delta;
                    if (Tile* t = findTile(lastRow, lastCol)) {
                        Vector2f pos = gridToScreen(lastRow, lastCol);
                        pos.y -= elevationMap[lastRow][lastCol];
                        t->shape.setPosition(pos);
                    }
                }

                // R — reset last tile fully
                if (kp->code == Keyboard::Key::R && lastRow >= 0) {
                    textureMap  [lastRow][lastCol] = -1;
                    elevationMap[lastRow][lastCol] =  0;
                    directionMap[lastRow][lastCol] =  0;
                    if (Tile* t = findTile(lastRow, lastCol))
                        t->shape.setPosition(gridToScreen(lastRow, lastCol));
                }
            }

            // ── Text input — digits only, always into shared buffer ───────────
            if (const auto* te = event->getIf<Event::TextEntered>()) {
                char c = (char)te->unicode;
                if (c >= '0' && c <= '9') {
                    // starting fresh clears armed state
                    if (texReady || dirReady) {
                        texReady = false;
                        dirReady = false;
                        inputBuffer = "";
                    }
                    inputBuffer += c;
                }
            }

            // ── Mouse ─────────────────────────────────────────────────────────
            if (const auto* mb = event->getIf<Event::MouseButtonPressed>())
            {
                if (mb->button == Mouse::Button::Left) {
                    Vector2f world = window.mapPixelToCoords(mb->position, gameView);
                    auto [r,c] = screenToTile(world.x, world.y);
                    if (r>=0 && r<ROWS && c>=0 && c<COLS) {
                        lastRow = r; lastCol = c;
                        if (texReady) textureMap  [r][c] = activeTexIdx;
                        if (dirReady) directionMap[r][c] = activeDir;
                    }
                }
                if (mb->button == Mouse::Button::Middle) {
                    panning        = true;
                    panStart       = mb->position;
                    viewCenterAtPan = gameView.getCenter();
                }
            }

            if (const auto* mb = event->getIf<Event::MouseButtonReleased>())
                if (mb->button == Mouse::Button::Middle) panning = false;

            if (const auto* mm = event->getIf<Event::MouseMoved>())
                if (panning) {
                    Vector2f delta = Vector2f(panStart - mm->position);
                    gameView.setCenter(viewCenterAtPan + delta);
                }

            if (const auto* sw = event->getIf<Event::MouseWheelScrolled>())
                gameView.zoom(sw->delta > 0 ? 0.9f : 1.1f);
        }

        // ── HUD text ─────────────────────────────────────────────────────────
        if (hasFont)
        {
            // top line — current armed mode + input buffer
            string top;
            if (!inputBuffer.empty())
                top = "Input: " + inputBuffer + "_   (Enter=arm texture  |  F=arm direction)";
            else if (texReady)
                top = "TEXTURE [" + to_string(activeTexIdx) + "] ARMED — click tiles to paint  |  type new number to change";
            else if (dirReady) {
                string dirName[] = {"none","down","left","right","up"};
                top = "DIRECTION [" + to_string(activeDir) + "=" + dirName[activeDir] + "] ARMED — click tiles to paint  |  type new number to change";
            }
            else
                top = "Type texture index + Enter  |  Type direction (0-4) + F  |  Up/Down=elevation  |  R=reset  |  Ctrl+S=save";
            topLine.setString(top);

            // bottom line — last tile info
            string bot;
            if (lastRow >= 0) {
                string dirName[] = {"none","down","left","right","up"};
                bot = "Last tile: (" + to_string(lastRow) + "," + to_string(lastCol) + ")"
                    + "  tex="  + to_string(textureMap  [lastRow][lastCol])
                    + "  elev=" + to_string(elevationMap[lastRow][lastCol])
                    + "  dir="  + to_string(directionMap[lastRow][lastCol])
                    + " (" + dirName[max(0,directionMap[lastRow][lastCol])] + ")";
            }
            botLine.setString(bot);
        }

        // ── Draw ─────────────────────────────────────────────────────────────
        window.clear(Color(15,15,20));
        window.setView(gameView);

        static const Color dirColors[] = {
            Color::Transparent,
            Color(0,255,0,100),    // 1 down   green
            Color(255,165,0,100),  // 2 left   orange
            Color(0,120,255,100),  // 3 right  blue
            Color(255,0,255,100)   // 4 up     magenta
        };

        for (auto& t : tiles)
        {
            int idx = textureMap[t.row][t.col];

            if (idx >= 0) {
                RectangleShape texTile({tileW, tileH*2.f});
                texTile.setTexture(&sheet);
                texTile.setTextureRect(indexToRect(idx));
                texTile.setPosition(t.shape.getPosition());
                window.draw(texTile);
            } else {
                window.draw(t.shape);
            }

            // direction colour overlay
            int dir = directionMap[t.row][t.col];
            if (dir > 0) {
                ConvexShape ov = t.shape;
                ov.setFillColor(dirColors[dir]);
                ov.setOutlineThickness(0.f);
                window.draw(ov);
            }

            // last-selected highlight
            if (t.row==lastRow && t.col==lastCol) {
                ConvexShape hl = t.shape;
                hl.setFillColor(Color(255,255,0,60));
                hl.setOutlineColor(Color::Yellow);
                hl.setOutlineThickness(1.5f);
                window.draw(hl);
            }
        }

        // HUD in screen space
        View uiView(FloatRect({0.f,0.f},{1200.f,700.f}));
        window.setView(uiView);
        window.draw(hudBg);
        if (hasFont) { window.draw(topLine); window.draw(botLine); }

        window.display();
    }
    return 0;
}