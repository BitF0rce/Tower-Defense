#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;
using namespace sf;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: ./pixel_picker <image_path>\n";
        return 1;
    }

    // Load texture and image (image for pixel data, texture for rendering)
    Texture texture(argv[1]);
    Image image = texture.copyToImage();

    Vector2u imgSize = texture.getSize();
    Sprite sprite(texture);

    // Open window same size as image (capped at 1200x800)
    unsigned int winW = min(imgSize.x, 1200u);
    unsigned int winH = min(imgSize.y, 800u);

    RenderWindow window(VideoMode({winW, winH}), "Pixel Picker - " + string(argv[1]));
    window.setFramerateLimit(60);

    // Scale sprite to fit window if image is larger
    float scaleX = (float)winW / imgSize.x;
    float scaleY = (float)winH / imgSize.y;
    sprite.setScale({scaleX, scaleY});

    // For drawing selection rectangle
    RectangleShape selection;
    selection.setFillColor(Color(255, 255, 0, 60));
    selection.setOutlineColor(Color::Yellow);
    selection.setOutlineThickness(1.f);

    Vector2i clickStart = {-1, -1};
    Vector2i clickEnd   = {-1, -1};
    bool dragging = false;

    // Font for overlay text
    Font font;
    bool hasFont = false;
    // Try common system font locations
    for (auto& path : {
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
    }) {
        if (font.openFromFile(path)) { hasFont = true; break; }
    }

    Text infoText(font);
    infoText.setCharacterSize(14);
    infoText.setFillColor(Color::Yellow);
    infoText.setOutlineColor(Color::Black);
    infoText.setOutlineThickness(1.f);
    infoText.setPosition({5.f, 5.f});
    infoText.setString("Click to get pixel coords.\nClick and drag to get IntRect.");

    string lastInfo = "Click to get pixel coords.\nClick and drag to get IntRect.";

    while (window.isOpen())
    {
        while (const optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();

            if (event->is<Event::KeyPressed>())
                window.close();

            // Mouse pressed — start drag
            if (const auto* mb = event->getIf<Event::MouseButtonPressed>())
            {
                if (mb->button == Mouse::Button::Left)
                {
                    // Convert screen pos back to image coords
                    int imgX = (int)(mb->position.x / scaleX);
                    int imgY = (int)(mb->position.y / scaleY);
                    clickStart = {imgX, imgY};
                    clickEnd   = {imgX, imgY};
                    dragging = true;

                    // Single click info
                    Color px = image.getPixel({(unsigned)imgX, (unsigned)imgY});
                    lastInfo =  "Pixel: (" + to_string(imgX) + ", " + to_string(imgY) + ")\n"
                                "RGBA: (" + to_string(px.r) + ", " + to_string(px.g) + ", "
                                          + to_string(px.b) + ", " + to_string(px.a) + ")\n"
                                "Drag to select a rect region.";
                    cout << "\n[CLICK] Pixel: (" << imgX << ", " << imgY << ")"
                         << "  RGBA: (" << (int)px.r << ", " << (int)px.g << ", "
                         << (int)px.b << ", " << (int)px.a << ")\n";
                }
            }

            // Mouse released — finalize rect
            if (const auto* mb = event->getIf<Event::MouseButtonReleased>())
            {
                if (mb->button == Mouse::Button::Left && dragging)
                {
                    int imgX = (int)(mb->position.x / scaleX);
                    int imgY = (int)(mb->position.y / scaleY);
                    clickEnd = {imgX, imgY};
                    dragging = false;

                    int rx = min(clickStart.x, clickEnd.x);
                    int ry = min(clickStart.y, clickEnd.y);
                    int rw = abs(clickEnd.x - clickStart.x);
                    int rh = abs(clickEnd.y - clickStart.y);

                    lastInfo =  "IntRect: ({" + to_string(rx) + ", " + to_string(ry) + "}, {"
                                             + to_string(rw) + ", " + to_string(rh) + "})\n"
                                "Top-left: (" + to_string(rx) + ", " + to_string(ry) + ")\n"
                                "Size: (" + to_string(rw) + " x " + to_string(rh) + ")";

                    cout << "[RECT]  IntRect({"  << rx << ", " << ry << "}, {"
                         << rw << ", " << rh << "})\n";
                }
            }

            // Mouse moved — update drag rect
            if (const auto* mm = event->getIf<Event::MouseMoved>())
            {
                if (dragging)
                {
                    int imgX = (int)(mm->position.x / scaleX);
                    int imgY = (int)(mm->position.y / scaleY);
                    clickEnd = {imgX, imgY};

                    int rx = min(clickStart.x, clickEnd.x);
                    int ry = min(clickStart.y, clickEnd.y);
                    int rw = abs(clickEnd.x - clickStart.x);
                    int rh = abs(clickEnd.y - clickStart.y);

                    lastInfo =  "Dragging...\n"
                                "IntRect: ({" + to_string(rx) + ", " + to_string(ry) + "}, {"
                                             + to_string(rw) + ", " + to_string(rh) + "})";
                }
            }
        }

        // Update selection rect in screen space
        if (clickStart.x >= 0 && clickEnd.x >= 0)
        {
            float sx = min(clickStart.x, clickEnd.x) * scaleX;
            float sy = min(clickStart.y, clickEnd.y) * scaleY;
            float sw = abs(clickEnd.x - clickStart.x) * scaleX;
            float sh = abs(clickEnd.y - clickStart.y) * scaleY;
            selection.setPosition({sx, sy});
            selection.setSize({sw, sh});
        }

        if (hasFont)
            infoText.setString(lastInfo);

        window.clear();
        window.draw(sprite);
        if (clickStart.x >= 0)
            window.draw(selection);
        if (hasFont)
            window.draw(infoText);
        window.display();
    }

    return 0;
}