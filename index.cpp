float playerHealth = 5;
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include "./Utility-Classes/displayQueue.h"
#include "./Screens/loading.h"
#include "./Game-Classes/Plot.h"
#include "./Game-Classes/Tower.h"

using namespace std;
using namespace sf;
bool goBackToMenu = false;

displayQueue queue;
EventHandler towerMenuEvents;
EventHandler anchorHover;
EventHandler mainMenuClicks;
EventHandler mapClicks;
EventHandler inGameEvents;
EventHandler overlayPromptEvents;
EventHandler shopEvents;
EventHandler instructionEvents;
EventHandler settingsEvents;


TypedDisplayQueue<Bullet , Enemy> bulletDisplayQueue;
TowerManager<Tower> allTowers;
int main()
{

    srand(static_cast<unsigned int>(time(0)));
    anchorHover.resume();
    inGameEvents.resume();
    ContextSettings settings;
    settings.antiAliasingLevel = 8;
    RenderWindow window(VideoMode({1200, 700}), "Tower Defense", Style::Default, State::Windowed, settings);
    gameWindow = &window;
    queue.pushBack(drawLoadingScreen);
    loadScreen(window);

    // View zoomOut;
    // zoomOut.setCenter({600, 350});
    // zoomOut.setSize({1200.f * 1.2f, 700.f * 1.2f});
    // window.setView(zoomOut);
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
                auto [tileRow, tileCol] = screenToTile(mousePos.x, mousePos.y);

                // cout << "Clicked: " << tileRow << " , " << tileCol <<" "<<mousePos.x<<" | "<<mousePos.y<<endl;

                if (tileRow >= 0 && tileRow < rows && tileCol >= 0 && tileCol < cols)
                    handler.handleClick(tileRow, tileCol);
            }
            anchorHover.handleEvents(*event);
            towerMenuEvents.handleEvents(*event);
            mainMenuClicks.handleEvents(*event);
            mapClicks.handleEvents(*event);
            inGameEvents.handleEvents(*event);
            overlayPromptEvents.handleEvents(*event);
            shopEvents.handleEvents(*event);
            instructionEvents.handleEvents(*event);
            settingsEvents.handleEvents(*event);
        }
        if (goBackToMenu) {
            goBackToMenu = false;
        }

        window.clear(Color(29, 29, 29));
        queue.runAll(window);
        bulletDisplayQueue.runAll(window);
        window.display();
    }
}