#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include "./Utility-Classes/displayQueue.h"
#include "./Screens/loading.h"
#include "./Game-Classes/Plot.h"

using namespace std;
using namespace sf;

displayQueue queue;
EventHandler towerMenuEvents;
EventHandler anchorHover;
int main()
{
    anchorHover.resume();
    ContextSettings settings;
    settings.antiAliasingLevel = 8;
    RenderWindow window(VideoMode({1200, 700}), "Tower Defense", Style::Default, State::Windowed, settings);
    queue.pushBack(drawLoadingScreen);
    loadScreen(window);
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }
            if (const auto *mouseMoved = event->getIf<Event::MouseButtonPressed>())
            {
                Vector2f mousePos = Vector2f(mouseMoved->position);

                float screenX = mousePos.x, screenY = mousePos.y;
                float dx = screenX - originX;
                float dy = screenY - originY;

                float col = (dx / (tileW / 2.f) + dy / (tileH / 2.f)) / 2.f;
                float row = (dy / (tileH / 2.f) - dx / (tileW / 2.f)) / 2.f;

                int tileCol = (int)floor(col) - 1;
                int tileRow = (int)floor(row);

                cout<< "Clicked: " << tileRow << " , " << tileCol<<endl;
                cout<<"<"<<screenX<<" , "<<screenY<<"> .. Dir = "<<grid[tileRow][tileCol]->getDirection()<<endl;
                if (tileRow >= 0 && tileRow < rows && tileCol >= 0 && tileCol < cols)
                {
                    handler.handleClick(tileRow, tileCol);
                }
                
            }
            anchorHover.handleEvents(*event);
            towerMenuEvents.handleEvents(*event);
        }
        window.clear(Color(29, 29, 29));
        queue.runAll(window);
        window.display();
    }
}