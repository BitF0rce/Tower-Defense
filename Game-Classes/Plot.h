#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../Screens/towerMenu.h"
#include "../Utility-Classes/displayQueue.h"
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

void towerMenu(Plot* object);
void hover(const Event& ev);
class anchorPlot : public Plot
{
    bool isOccupied = false;
    RectangleShape projection;
    public:
    bool drawProjection = false;
    anchorPlot()
    {
        onclick = towerMenu;
        plotBase.setTexture(&towerTexture);
        Vector2u size = towerHoverTexture.getSize();
        projection.setSize({200.f, 400.f});
        projection.setTextureRect({{400, 0}, {700, (int)size.y-100}});
        projection.setScale({0.2f,0.2f});
        projection.setTexture(&towerHoverTexture);
        projection.setScale({0.3, 0.3f});
        projection.setOrigin({0, projection.getSize().y});
        anchorHover.addEvent(hover);
    }
    void draw(RenderWindow &window)
    {
        window.draw(plotBase);
        if (drawProjection)
            window.draw(projection);
    }
    void startProjection()
    {
        drawProjection = true;
    }
    void stopProject(){
        drawProjection = false;
    }
    void setPosition(Vector2f pos){
        plotBase.setPosition(pos);
        projection.setPosition(pos);
        cout<<"Projection : "<<projection.getPosition().x<<" , "<<projection.getPosition().y<<endl;
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
        }
    }
    else if (ev.getIf<Event::MouseMoved>())
    {
        Vector2f mousePos = Vector2f(ev.getIf<Event::MouseMoved>()->position);
        float screenX = mousePos.x , screenY = mousePos.y;
        float dx = screenX - originX;
        float dy = screenY - originY;

        float col = (dx / (tileW / 2.f) + dy / (tileH / 2.f)) / 2.f;
        float row = (dy / (tileH / 2.f) - dx / (tileW / 2.f)) / 2.f;

        int tileCol = (int)floor(col)-1;
        int tileRow = (int)floor(row);
        if (tileRow == 24 && tileCol == 27)
        {
            anchorPlot* plot = dynamic_cast<anchorPlot*>(grid[24][27]);
            plot->drawProjection = true;
        }
        else{
            anchorPlot* plot = dynamic_cast<anchorPlot*>(grid[24][27]);
            plot->drawProjection = false;
        }
    }
}

void hover(const Event& ev){

    if (ev.getIf<Event::MouseMoved>())
    {
        Vector2f mousePos = Vector2f(ev.getIf<Event::MouseMoved>()->position);
        float screenX = mousePos.x , screenY = mousePos.y;
        float dx = screenX - originX;
        float dy = screenY - originY;

        float col = (dx / (tileW / 2.f) + dy / (tileH / 2.f)) / 2.f;
        float row = (dy / (tileH / 2.f) - dx / (tileW / 2.f)) / 2.f;

        int tileCol = (int)floor(col)-1;
        int tileRow = (int)floor(row);
        if (tileRow == 24&& tileCol == 27 || tileRow==27 && tileCol== 23)
        {
            // cerr<<"HOVERING...";
            anchorPlot* plot = dynamic_cast<anchorPlot*>(grid[tileRow][tileCol]);
            plot->drawProjection = true;
        }
        else{
            anchorPlot* plot = dynamic_cast<anchorPlot*>(grid[24][27]);
            plot->drawProjection = false;
            plot = dynamic_cast<anchorPlot*>(grid[27][23]);
            plot->drawProjection = false;

        }
    }
}

void towerMenu(Plot *object)
{
    stopDrawing = false;
    handler.pause();
    initiateTowerMenu(object->getPosition());
    queue.pushBack(drawTowerMenu);
    towerMenuEvents.addEvent(click);
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

            if (i == 24&& j == 27 || i==27 && j== 23)
            {
                grid[i][j] = new anchorPlot();
            }
            else if (i == 20 || i == 21 || j == 21 || j == 22)
            {
                grid[i][j] = new pathPlot();
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
}

int getDirection(float screenX, float screenY)
{
    float dx = screenX - originX;
    float dy = screenY - originY;

    float col = (dx / (tileW / 2.f) + dy / (tileH / 2.f)) / 2.f;
    float row = (dy / (tileH / 2.f) - dx / (tileW / 2.f)) / 2.f;

    int tileCol = (int)floor(col) - 1;
    int tileRow = (int)floor(row);
    int r = 0;
    if (tileRow >= 0 && tileRow < rows && tileCol >= 0 && tileCol < cols)
    {
        r = grid[tileRow][tileCol]->getDirection();
    }
    return r;
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