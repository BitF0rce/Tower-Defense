#include <iostream>
#include <SFML/Graphics.hpp>
#include "../Utility-Classes/displayQueue.h"
#include "../Game-Classes/Plot.h"

using namespace sf;
using namespace std;
extern EventHandler mainMenuClicks;
extern EventHandler mapClicks;

bool stopMenu = false;
bool stopMap = false;

bool drawMap(RenderWindow &window);
void loadMap();
void handleMapEvents(const Event &ev);

Texture mainMenuItems[6] = {
    Texture("./Assets/Menu-Items/menu_0.png"),
    Texture("./Assets/Menu-Items/menu_1.png"),
    Texture("./Assets/Menu-Items/menu_2.png"),
    Texture("./Assets/Menu-Items/menu_3.png"),
    Texture("./Assets/Menu-Items/menu_4.png"),
    Texture("./Assets/Menu-Items/menu_5.png")};
RectangleShape buttonHitboxes[6];

Sprite mainMenu(mainMenuItems[0]);
void loadMenu()
{
    mainMenu.setScale({0.4264f, 0.4557f});

    buttonHitboxes[0].setPosition({494.f, 254.f});
    buttonHitboxes[1].setPosition({490.f, 324.f});
    buttonHitboxes[2].setPosition({508.f, 395.f});
    buttonHitboxes[3].setPosition({509.f, 464.f});
    buttonHitboxes[4].setPosition({517.f, 530.f});

    buttonHitboxes[0].setSize({212.f, 56.f});
    buttonHitboxes[1].setSize({221.f, 55.f});
    buttonHitboxes[2].setSize({184.f, 54.f});
    buttonHitboxes[3].setSize({183.f, 55.f});
    buttonHitboxes[4].setSize({167.f, 57.f});
}

void handleMainEvents(const Event &ev)
{
    if (const auto click = ev.getIf<Event::MouseMoved>())
    {
        Vector2f pos = {(float)(click->position).x, (float)(click->position).y};
        bool hovering = false;
        for (int i = 0; i < 5; i++)
        {
            if (buttonHitboxes[i].getGlobalBounds().contains(pos))
            {
                mainMenu.setTexture(mainMenuItems[i + 1]);
                hovering = true;
                break;
            }
        }
        if (!hovering)
        {
            mainMenu.setTexture(mainMenuItems[0]);
        }
    }
    if (const auto click = ev.getIf<Event::MouseButtonPressed>())
    {
        int target = 0;
        Vector2f pos = {(float)(click->position).x, (float)(click->position).y};
        for (int i = 0; i < 5; i++)
        {
            if (buttonHitboxes[i].getGlobalBounds().contains(pos))
            {
                target = i;
                break;
            }
        }
        switch (target)
        {
        case 0:
            stopMenu = true;
            mainMenuClicks.pause();
            loadMap();
            queue.pushBack(drawMap);
            mapClicks.addEvent(handleMapEvents);
            mapClicks.resume();
            stopMap = false;
        }
    }
}

bool displayMenu(RenderWindow &window)
{
    window.draw(mainMenu);
    return stopMenu;
}

Texture map("./Assets/Map-Items/map_0.png");
Texture homeButtonTexture("./Assets/Map-Items/backHome.png");
Sprite mapSprite(map);
CircularButton backToMenu;
RectangleShape mapHitBoxes[3];
void loadMap()
{
    float r = 40.f;
    mapSprite.setScale({0.4166f, 0.4755f});
    backToMenu.rad = r;
    backToMenu.base.setRadius(r);
    backToMenu.visual.setRadius(r);
    backToMenu.base.setOrigin({r, r});
    backToMenu.visual.setOrigin({r, r});
    backToMenu.visual.setTexture(&homeButtonTexture);
    backToMenu.visual.setTextureRect({{240,57} , {226 , 243}});

    backToMenu.base.setPosition({1200 - r - 20, 700 - r - 20});
    backToMenu.visual.setPosition(backToMenu.base.getPosition());
    backToMenu.position = backToMenu.base.getPosition();

    mapHitBoxes[0].setPosition({209, 462});
    mapHitBoxes[1].setPosition({679, 118});
    mapHitBoxes[2].setPosition({194, 38});

    mapHitBoxes[0].setSize({96,142});
    mapHitBoxes[1].setSize({87,150});
    mapHitBoxes[2].setSize({172,183});

}

void handleMapEvents(const Event &ev)
{
    if (const auto click = ev.getIf<Event::MouseMoved>())
    {
        Vector2f pos = {(float)(click->position).x, (float)(click->position).y};
        if (backToMenu.base.getGlobalBounds().contains(pos))
        {
            backToMenu.hovering = true;
        }
        else
        {
            backToMenu.visual.setPosition(backToMenu.base.getPosition());
            backToMenu.hovering = false;
        }
    }
    if (const auto click = ev.getIf<Event::MouseButtonPressed>())
    {
        Vector2f pos = {(float)(click->position).x, (float)(click->position).y};
        if (backToMenu.base.getGlobalBounds().contains(pos))
        {
            backToMenu.hovering = false;
            mapClicks.pause();
            stopMap = true;
            stopMenu = false;
            queue.pushBack(displayMenu);
            mainMenuClicks.resume();
        }
        else if(mapHitBoxes[0].getGlobalBounds().contains(pos)){
            cerr<<"Level 1....\n";
            mapClicks.pause();
            stopMap = true;
            loadGrid();
            queue.pushBack(drawGrid);
            loadLevel1();
            queue.pushBack(drawLevel1);
        }
        else if(mapHitBoxes[1].getGlobalBounds().contains(pos)){
            cerr<<"Level 2....\n";
        }
        else if(mapHitBoxes[2].getGlobalBounds().contains(pos)){
            cerr<<"Level 3....\n";
        }
    }
}

bool drawMap(RenderWindow &window)
{
    if (backToMenu.hovering)
    {
        if (backToMenu.visual.getPosition().y >= backToMenu.position.y - backToMenu.rad)
        {
            if (backToMenu.moveClock.getElapsedTime().asMilliseconds() >= 10)
            {
                backToMenu.visual.move({0, -2});
                backToMenu.moveClock.restart();
            }
        }
    }

    window.draw(mapSprite);
    window.draw(backToMenu.visual);
    return stopMap;
}
