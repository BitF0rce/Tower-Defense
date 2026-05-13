#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <math.h>
#include <SFML/Window.hpp>
#include "../Utility-Classes/RoundedRectangle.cpp"
#include "../Utility-Classes/displayQueue.h"
#include "../Game-Classes/Plot.h"
#include "../Game-Classes/Enemy.h"
#include "main.h"
using namespace std;
using namespace sf;

extern displayQueue queue;
extern EventHandler mainMenuClicks;

Font myFont("./Fonts/GameBox-Regular.ttf");
RoundedRectangle progressContainer;
RoundedRectangle progressBar;
Clock counter;
float x = 0, progress = 0;
Text bottle[3] = {Text(myFont), Text(myFont), Text(myFont)};
Text neck[5] = {Text(myFont), Text(myFont), Text(myFont), Text(myFont) , Text(myFont)};

bool loadScreen(RenderWindow &win)
{
    Vector2f dimensions = {(float)win.getSize().x, (float)win.getSize().y};
    progressContainer = RoundedRectangle(dimensions.x * 0.5f, dimensions.y * 0.00f, 10);
    progressContainer.setFillColor(Color::White);
    progressBar = RoundedRectangle(0, dimensions.y * 0.000f, 10);
    progressBar.setFillColor(Color::Green);
    progressContainer.setPosition({(dimensions.x - progressContainer.getSize().x) / 2, (dimensions.y - progressContainer.getSize().y) - 100.0f});
    progressBar.setPosition(progressContainer.getPosition());

    // bottle[6] =, Text(myFont), Text(myFont), Text(myFont), Text(myFont), Text(myFont)};
    for (int i = 0; i < 3; i++)
    {
        bottle[i].setFont(myFont);
        bottle[i].setCharacterSize(40);
    }
    for (int i = 0; i < 5; i++)
    {
        neck[i].setFont(myFont);
        neck[i].setCharacterSize(40);
    }
    bottle[0].setString('B');
    bottle[1].setString('I');
    bottle[2].setString('T');
    // neck[4] = {Text(myFont), Text(myFont), Text(myFont), Text(myFont)};
    neck[0].setString('F');
    neck[1].setString('0');
    neck[2].setString('R');
    neck[3].setString('C');
    neck[4].setString('E');

    for (int i = 0; i < 3; i++)
    {
        bottle[i].setPosition({-10.0f - (i * 40.0f), dimensions.y / 2});
    }
    for (int i = 0; i < 5; i++)
    {
        neck[i].setPosition({dimensions.x + 10 + (i * 40.0f), dimensions.y / 2});
    }
    progress = 0;
    x = 0;
    return true;
}

void drawBottleNeck(RenderWindow &win)
{
    float screenMid = 520.f;
    float charW = 35.f;
    float gap = 20.f;     
    float stiffness = 0.08f;
    for (int i = 0; i < 3; i++)
    {
        float targetX = screenMid - gap - ((2 - i) * charW); 
        float currentX = bottle[i].getPosition().x;
        bottle[i].move({(targetX - currentX) * stiffness, 0});
    }

    for (int i = 0; i < 5; i++)
    {
        float targetX = screenMid + gap + (i * charW);
        float currentX = neck[i].getPosition().x;
        neck[i].move({(targetX - currentX) * stiffness, 0});
    }
    for (int i = 0; i < 3; i++)
        win.draw(bottle[i]);
    for (int i = 0; i < 5; i++)
        win.draw(neck[i]);
}

float drawProgressBar(RenderWindow &win)
{

    if (counter.getElapsedTime().asMilliseconds() > 20)
    {
        x += 0.005f;
        progress = 100 * (1 - pow(1 - x, 3));
        progressBar.setSize({(progress / 100) * progressContainer.getSize().x, progressBar.getSize().y});
        counter.restart();
    }
    progressContainer.draw(win);
    progressBar.draw(win);
    return x;
}

bool drawLoadingScreen(RenderWindow &window)
{
    drawBottleNeck(window);
    float i = drawProgressBar(window);
    if (i >= 1)
    {
        window.clear();
        // loadGrid();
        // queue.pushBack(drawGrid);
        // queue.pushBack(drawEnemy);
        loadMenu();
        queue.pushBack(displayMenu);
        mainMenuClicks.addEvent(handleMainEvents);
        mainMenuClicks.resume();
        return true;
    }
    else
        return false;
}