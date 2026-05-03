#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../Screens/towerMenu.h"
#include "../Utility-Classes/displayQueue.h"
#include "Tower.h"
using namespace std;
using namespace sf;

extern displayQueue queue;
extern EventHandler towerMenuEvents;
extern EventHandler anchorHover;

Texture Path("./Assets/isometric/separated-images/tile_114.png");
Texture Path2("./Assets/isometric/separated-images/tile_009.png");
Texture towerTexture("./Assets/isometric/separated-images/tile_063.png");
Texture towerHoverTexture("./Assets/outlineTexture.png");

int getDirection(float screenX, float screenY);
class Plot;
class gridClickHandler;

float tileW = 50,
      tileH = 25,
      originX = 560.0f,
      originY = -300.0f; // Grid Height = (41 + 41) * (32 / 2) = 80 * 16 = 1280px. Screen Height = 700. diff = 580. diff/2 = 290.
const int rows = 52,
          cols = 52;
Texture grassTexture("./Assets/grass.png");
void placeHolderFunction(Plot *object)
{
    // DOES NOTHING
}

pair<int,int> screenToTile(float screenX, float screenY)
{
    float dx = screenX - (originX + tileW / 2.f);
    float dy = screenY - originY;

    float fcol = (dx / (tileW / 2.f) + dy / (tileH / 2.f)) / 2.f;
    float frow = (dy / (tileH / 2.f) - dx / (tileW / 2.f)) / 2.f;

    int baseRow = (int)floor(frow);
    int baseCol = (int)floor(fcol);

    // Check this tile and its 3 neighbours, pick closest center
    int bestRow = baseRow, bestCol = baseCol;
    float bestDist = __FLT_MAX__;

    for (int dr = 0; dr <= 1; dr++)
    {
        for (int dc = 0; dc <= 1; dc++)
        {
            int r = baseRow + dr;
            int c = baseCol + dc;

            // Get screen center of this candidate tile
            float cx = (originX + tileW / 2.f) + (c - r) * (tileW / 2.f);
            float cy = originY + (c + r) * (tileH / 2.f) + tileH / 2.f;

            float dist = (screenX - cx) * (screenX - cx) + (screenY - cy) * (screenY - cy);
            if (dist < bestDist)
            {
                bestDist = dist;
                bestRow = r;
                bestCol = c;
            }
        }
    }

    return { bestRow, bestCol };
}

class Plot
{
protected:
    ConvexShape plotBase;
    int location[2];
    void (*onclick)(Plot *object) = placeHolderFunction;
    int direction = 0; // 0-> Forbidden , 1-> Down mv(x,x/2) , 2-> , 3->Left , 4-> Up

public:
    Plot()
    {
        plotBase.setPointCount(6);
        plotBase.setPoint(0, {tileW / 2.f, 0.f});
        plotBase.setPoint(1, {tileW, tileH});
        plotBase.setPoint(2, {tileW, tileH + (0.4f * tileH)});
        plotBase.setPoint(3, {tileW / 2.f, tileH * 2});
        plotBase.setPoint(4, {0.f, tileH + (0.4f * tileH)});
        plotBase.setPoint(5, {0.f, tileH});
    }
    const Texture *getTexture()
    {
        return plotBase.getTexture();
    }
    void setDirection(int n) { direction = n; }
    virtual int getDirection() { return direction; }
    void setTexture(Texture *tex)
    {
        plotBase.setTexture(tex);
    }
    virtual void setTextureRect(IntRect rect)
    {
        plotBase.setTextureRect(rect);
    }
    virtual void setPosition(Vector2f pos)
    {
        plotBase.setPosition(pos);
    }
    void move(Vector2f mv)
    {
        plotBase.move(mv);
    }
    virtual void draw(RenderWindow &window)
    {
        window.draw(plotBase);
    }
    friend void loadGrid();
    friend class gridClickHandler;
    void addClickReaction(void (*tFunc)(Plot *object))
    {
        onclick = tFunc;
    }
    Vector2f getPosition()
    {
        return plotBase.getPosition();
    }
    int getRow(){return location[0];}
    int getCol(){return location[1];}
};

class simplePlot : public Plot
{
public:
    simplePlot()
    {
        plotBase.setTexture(&Path);
    }
    void draw(RenderWindow &window)
    {
        window.draw(plotBase);
    }
};

class pathPlot : public Plot
{
public:
    pathPlot()
    {
        plotBase.setTexture(&Path2);
        plotBase.move({0.0f, 7.0f});
    }
    void draw(RenderWindow &window)
    {
        window.draw(plotBase);
    }
};

class gridClickHandler
{
    Plot **ptr;
    int count = 0;
    bool isPaused = false;

public:
    gridClickHandler()
    {
        ptr = nullptr;
        count = 0;
    }
    void pause()
    {
        isPaused = true;
    }
    void resume()
    {
        isPaused = false;
    }
    void addBlock(Plot *candidate)
    {
        Plot **temp = new Plot *[count + 1];
        for (int i = 0; i < count; i++)
        {
            temp[i] = ptr[i];
        }
        if (ptr != nullptr)
        {
            delete[] ptr;
        }
        temp[count] = candidate;
        ptr = temp;
        count++;
    }
    void handleClick(int r, int c)
    {
        if (isPaused)
        {
            return;
        }
        for (int i = 0; i < count; i++)
        {
            if (ptr[i]->location[0] == r && ptr[i]->location[1] == c)
            {
                ptr[i]->onclick(ptr[i]);
            }
        }
    }
};
class EnemyManager
{
    int **info = nullptr;
    Tower** towersArray;
    int count = 0;

public:
    void pushBack(Vector2i loc, int radius,Tower* ptr)
    {
        int **temp = new int *[count + 1];
        for (int i = 0; i < count; i++)
        {
            temp[i] = new int[3];
            for (int j = 0; j < 3; j++)
            temp[i][j] = info[i][j];
        }
        temp[count] = new int[3];
        temp[count][0] = loc.x;
        temp[count][1] = loc.y;
        temp[count][2] = radius;
        for (int i = 0; i < count; i++)
        {
            delete[] info[i];
        }
        delete[] info;
        info = temp;
        Tower** tempT = new Tower*[count];
        for(int i=0;i<count;i++){
            tempT[i]= towersArray[i];
        }
        tempT[count] = ptr;
        delete[] towersArray;
        towersArray = tempT;
        count++;
    }
    void poll(Vector2i loc, Vector2f pos, float speed)
    {
        int cRow = loc.x, cCol = loc.y;
        for (int i = 0; i < count; i++)
        {
            if (cRow <= info[i][0] + info[i][2] && cRow >= info[i][0] - info[i][2])
            {
                if (cCol <= info[i][1] + info[i][2] && cCol >= info[i][1] - info[i][2])
                {
                    towersArray[i]->attack(pos, speed);
                }
            }
        }
    }
};

EnemyManager masterManager;

void towerMenu(Plot *object);
void hover(const Event &ev);
class anchorPlot : public Plot
{
    bool isOccupied = false;
    RectangleShape projection;
    Tower *towerPointer = nullptr;

public:
    bool drawProjection = false;
    friend void click(const Event &ev);
    anchorPlot()
    {
        onclick = towerMenu;
        plotBase.setTexture(&towerTexture);
        Vector2u size = towerHoverTexture.getSize();
        projection.setSize({200.f, 400.f});
        projection.setTextureRect({{400, 0}, {700, (int)size.y - 100}});
        projection.setTexture(&towerHoverTexture);
        projection.setScale({0.2, 0.2f});
        projection.setOrigin({0, projection.getSize().y});
    }
    void draw(RenderWindow &window)
    {
        window.draw(plotBase);
        if (drawProjection)
            window.draw(projection);
        if (towerPointer != nullptr)
            towerPointer->draw(window);
    }
    void startProjection()
    {
        drawProjection = true;
    }
    void stopProject()
    {
        drawProjection = false;
    }
    void setPosition(Vector2f pos)
    {
        plotBase.setPosition(pos);
        projection.setPosition({pos.x , pos.y + tileH*1.5f});
        cout << "Projection : " << projection.getPosition().x << " , " << projection.getPosition().y << endl;
    }
    void buildTower(int a)
    {
        if (a == 0)
        {
            towerPointer = new IceTower(location[0], location[1]);
            Vector2f pos = this->getPosition();
            towerPointer->setPosition({pos.x + tileW / 2.f, pos.y + tileH*1.5f});
            masterManager.pushBack({location[0] , location[1]}, towerPointer->getRadius(), towerPointer);
            isOccupied = true;
        }
    }
    bool occupationStatus(){
        return isOccupied;
    }
    void invokeTower(Vector2f enemyPos, float enemySpeed){
        cout<<"Tower Invoked..........\n";
    }
    int getRadius(){
        cerr<<"Inside Plot...";

        return  towerPointer->getRadius();
    }
};

gridClickHandler handler;
Plot *grid[rows][cols];

void click(const Event &ev)
{
    if (ev.getIf<Event::KeyPressed>())
    {
        cerr << "Menu OVER\n";
        stopDrawing = true;
        handler.resume();
        towerMenuEvents.pause();
        anchorHover.resume();
    }
    else if (ev.getIf<Event::MouseButtonPressed>())
    {
        Vector2f mousePos = Vector2f(ev.getIf<Event::MouseButtonPressed>()->position);
        if (tower.getGlobalBounds().contains(mousePos))
        {
            cerr << "Arrow Tower Has been Selected...\n";
            anchorPlot *plot = dynamic_cast<anchorPlot *>(grid[openAnchorRow][openAnchorCol]);
            if (!plot->isOccupied)
            {
                plot->buildTower(0);

            }
            stopDrawing = true;
            handler.resume();
            towerMenuEvents.pause();
            anchorHover.resume();
        }
    }
}

void hover(const Event &ev)
{
    static anchorPlot *lastHovered = nullptr;
    if (ev.getIf<Event::MouseMoved>())
    {
        Vector2f mouse = Vector2f(ev.getIf<Event::MouseMoved>()->position);
        auto [tileRow, tileCol] = screenToTile(mouse.x, mouse.y);

        if (lastHovered != nullptr)
        {
            lastHovered->drawProjection = false;
            lastHovered = nullptr;
        }

        if (tileRow >= 0 && tileRow < rows && tileCol >= 0 && tileCol < cols)
        {
            anchorPlot *plot = dynamic_cast<anchorPlot *>(grid[tileRow][tileCol]);
            if (plot != nullptr)
            {
                lastHovered = plot; // always update lastHovered
                if ((!plot->occupationStatus() && stopDrawing))
                    plot->drawProjection = true; // only show if unoccupied
            }
        }
    }
}
void towerMenu(Plot *object)
{
    stopDrawing = false;
    openAnchorRow = object->getRow();
    openAnchorCol = object->getCol();
    handler.pause();
    initiateTowerMenu(object->getPosition());
    queue.pushBack(drawTowerMenu);
    towerMenuEvents.resume();
    anchorHover.pause();
}

// Working Starts here.
void loadGrid()
{
    for (int i = 0; i < rows; i++)
    {

        for (int j = 0; j < cols; j++)
        {
            bool isAnchor;

            float screenX = originX + (j - i) * (tileW / 2);
            float screenY = originY + (j + i) * (tileH / 2);

            if (i == 19 && j == 14 || i == 22 && j == 23 || i==12 && j== 24)
            {
                grid[i][j] = new anchorPlot();
            }
            else if (i == 20 || i == 21 || j == 21 || j == 22)
            {
                grid[i][j] = new pathPlot();
                if(j==21 || j==22){
                    grid[i][j]->direction = 2;
                }
                else{
                    grid[i][j]->direction = 1;
                }
            }
            else
            {
                grid[i][j] = new simplePlot();
            }
            handler.addBlock((grid[i][j]));
            grid[i][j]->setPosition({screenX, screenY});
            grid[i][j]->setTextureRect(IntRect({0, 0}, {32, 32}));
            grid[i][j]->location[0] = i;
            grid[i][j]->location[1] = j;
        }
    }
    anchorHover.addEvent(hover);
    towerMenuEvents.addEvent(click);
}

int getDirection(float screenX, float screenY)
{
    auto [tileRow, tileCol] = screenToTile(screenX, screenY);
    if (tileRow >= 0 && tileRow < rows && tileCol >= 0 && tileCol < cols)
        return grid[tileRow][tileCol]->getDirection();
    return 0;
}

bool drawGrid(RenderWindow &window)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            grid[i][j]->draw(window);
        }
    }
    return false;
}