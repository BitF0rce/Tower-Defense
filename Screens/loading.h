#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <math.h>
#include <SFML/Window.hpp>
#include "../Utility-Classes/RoundedRectangle.cpp"
#include"../Utility-Classes/displayQueue.h"
#include "../Game-Classes/Plot.h"
#include "../Game-Classes/Enemy.h"
using namespace std;
using namespace sf;


extern displayQueue queue;
Font myFont("/home/shaaf/Desktop/Tower-Defense/Fonts/GameBox-Regular.ttf");
RoundedRectangle progressContainer;
RoundedRectangle progressBar;
Clock counter;
float x=0 , progress=0;
Text bottle[6] = {Text(myFont) , Text(myFont),Text(myFont),Text(myFont),Text(myFont),Text(myFont)};
Text neck[4] = {Text(myFont),Text(myFont),Text(myFont),Text(myFont)};

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
    for (int i = 0; i < 6; i++) {
        bottle[i].setFont(myFont);
        bottle[i].setCharacterSize(40);
    }
    for (int i = 0; i < 4; i++) {
        neck[i].setFont(myFont);
        neck[i].setCharacterSize(40);
    }
    bottle[0].setString('E');
    bottle[1].setString('L');
    bottle[2].setString('T');
    bottle[3].setString('T');
    bottle[4].setString('O');
    bottle[5].setString('B');
    // neck[4] = {Text(myFont), Text(myFont), Text(myFont), Text(myFont)};
    neck[0].setString('N');
    neck[1].setString('E');
    neck[2].setString('C');
    neck[3].setString('K');
    for (int i = 0; i < 6; i++)
    {
        bottle[i].setPosition({-10.0f - (i * 40.0f), dimensions.y / 2});
    }
    for (int i = 0; i < 4; i++)
    {
        neck[i].setPosition({dimensions.x + 10 + (i * 40.0f), dimensions.y / 2});
    }
    progress = 0;
    x = 0;
    return true;
}

void drawBottleNeck(RenderWindow &win)
{

    if (counter.getElapsedTime().asMilliseconds() > 15){
    
    Vector2f dimensions = {(float)win.getSize().x, (float)win.getSize().y};
    for (int i = 0; i < 6; i++)
    {
        if (bottle[0].getPosition().x >= dimensions.x / 2 - 20)
        {
            break;
        }
        bottle[i].move({4.0f, 0});
    }

    for (int i = 0; i < 4; i++)
    {
        if (neck[0].getPosition().x <= dimensions.x / 2 + 20)
        {
            break;
        }
        neck[i].move({-4.0f, 0});
    }
}
    for (int i = 0; i < 6; i++)
    {
        win.draw(bottle[i]);
    }
    for (int i = 0; i < 4; i++)
    {
        win.draw(neck[i]);
    }
}

float drawProgressBar(RenderWindow& win)
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

bool drawLoadingScreen(RenderWindow& window){
    drawBottleNeck(window);
    float i = drawProgressBar(window);
    if(i >= 0.05)
    {
        window.clear();
        loadGrid();
        queue.pushBack(drawGrid);
        queue.pushBack(drawEnemy);
        return true;
    }
    else return false;
}