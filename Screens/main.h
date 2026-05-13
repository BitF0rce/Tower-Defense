#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include<string>
#include "../Utility-Classes/displayQueue.h"
#include "../Game-Classes/Plot.h"
#include"shop.h"
#include "MapCarousel.h"
#include "instructions.h"
#include "settings.h"
using namespace sf;
using namespace std;


RenderWindow *gameWindow;
extern EventHandler mainMenuClicks;
extern EventHandler mapClicks;
extern EventHandler shopEvents;

MapCarousel mapSelector;
Texture mapThumbnails[3] = {Texture("./Assets/Map-Items/level_1.png") , Texture("./Assets/Map-Items/level_2.png") , Texture("./Assets/Map-Items/level_3.png")};
Texture arrowTex;


bool stopMenu = false;
bool stopMap = false;
int tempGold = 0;

bool drawMap(RenderWindow &window);
void loadMap();
void handleMapEvents(const Event &ev);
CircularButton backToMenu;
Texture homeButtonTexture("./Assets/Map-Items/backHome.png");

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
    loadBankData();
    loadPoints();

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

bool stopShopMenu;
Texture shopBG_Texture("./Assets/shopBG.png");
Texture shopBox_Texture("./Assets/shopBox.png");
Texture shopTower_Texture("./Assets/towers.png");
Texture upgradeBtn_Texture("./Assets/upgradeBTN.png");

RectangleShape shopBG;
RectangleShape shopItems[5];
RectangleShape shopDisplayTowers[5];
CircularButton shopUpgrades[5];


void handleShopEvents(const Event &ev);
void loadShop()
{
    loadHomeButton();
    shopBG.setTexture(&shopBG_Texture);
    shopBG.setSize({2816, 1505});
    shopBG.setScale({1200.f / 2816.f, 700.f / 1505.f});
    shopEvents.addEvent(handleShopEvents);

    for (int i = 0; i < 5; i++)
    {
        shopItems[i].setTexture(&shopBox_Texture);
        shopItems[i].setSize({287, 869});
        shopItems[i].setScale({200.f / 287, 600.f / 869.f});
        shopItems[i].setPosition({(200.f + 20.f) * i + 50.f, 50.f});
    }

    for (int i = 0; i < 5; i++)
    {
        shopDisplayTowers[i].setTexture(&shopTower_Texture);
        shopDisplayTowers[i].setSize({186, 286});
        shopDisplayTowers[i].setScale({0.6f, 0.6f});
    }

    shopDisplayTowers[0].setTextureRect({{14, 14}, {126, 151}});
    shopDisplayTowers[1].setTextureRect({{148, 13}, {126, 156}});
    shopDisplayTowers[2].setTextureRect({{17, 191}, {123, 159}});
    shopDisplayTowers[3].setTextureRect({{280, 10}, {122, 156}});
    shopDisplayTowers[4].setTextureRect({{411, 184}, {122, 156}});

    shopDisplayTowers[0].setPosition({95, 200});
    shopDisplayTowers[1].setPosition({317, 200});
    shopDisplayTowers[2].setPosition({536, 200});
    shopDisplayTowers[3].setPosition({759, 200});
    shopDisplayTowers[4].setPosition({981, 200});

    float btnRad = 25.f;
    for (int i = 0; i < 5; i++)
    {
        shopUpgrades[i].rad = btnRad;
        shopUpgrades[i].base.setRadius(btnRad);
        shopUpgrades[i].visual.setRadius(btnRad);
        shopUpgrades[i].base.setOrigin({btnRad, btnRad});
        shopUpgrades[i].visual.setOrigin({btnRad, btnRad});
        shopUpgrades[i].visual.setTexture(&upgradeBtn_Texture);
        shopUpgrades[i].visual.setTextureRect({{2, 41}, {286, 305}});
        Vector2f boxPos = shopItems[i].getPosition();
        float centerX = boxPos.x + (200.f / 2);
        float bottomY = boxPos.y + 495.f;

        shopUpgrades[i].base.setPosition({centerX, bottomY});
        shopUpgrades[i].visual.setPosition({centerX, bottomY});
        shopUpgrades[i].position = {centerX, bottomY};
    }

    for (int i = 0; i < 5; i++)
    {
        shopInfoBoxes[i].setSize({589.f, 332.f});                
        // shopInfoBoxes[i].setFillColor(Color(0, 0, 0, 0));     
        // shopInfoBoxes[i].setOutlineColor(Color(200, 180, 100));
        // shopInfoBoxes[i].setOutlineThickness(2.f);
        shopInfoBoxes[i].setTexture(&scrollTexture);
        shopInfoBoxes[i].setTextureRect({{32, 27}, {589, 332}});
        shopInfoBoxes[i].setScale({137.f/589.f , 81.f/332.f});

        Vector2f itemPos = shopItems[i].getPosition();
        float boxX = itemPos.x + (200.f / 2) - (137.f/2.f);
        float boxY = 420.f;              
        shopInfoBoxes[i].setPosition({boxX, boxY});

        shopLevelTexts[i].setFont(infoFont);
        shopLevelTexts[i].setString("Level 1");
        shopLevelTexts[i].setCharacterSize(20);
        shopLevelTexts[i].setFillColor(Color::White);
        shopLevelTexts[i].setOutlineColor(Color::Black);
        shopLevelTexts[i].setOutlineThickness(1.f);

        FloatRect boxBounds = shopInfoBoxes[i].getGlobalBounds();
        FloatRect textBounds = shopLevelTexts[i].getLocalBounds();
        shopLevelTexts[i].setPosition({boxBounds.position.x + (boxBounds.size.x / 2) - (textBounds.size.x / 2),
                                       boxBounds.position.y + 15.f});

        shopPriceTexts[i].setFont(infoFont);
        shopPriceTexts[i].setString("Price : 999G");
        shopPriceTexts[i].setCharacterSize(19);
        shopPriceTexts[i].setFillColor(Color(255, 215, 0));
        shopPriceTexts[i].setOutlineColor(Color::Black);
        shopPriceTexts[i].setOutlineThickness(1.f);

        textBounds = shopPriceTexts[i].getLocalBounds();
        shopPriceTexts[i].setPosition({boxBounds.position.x + (boxBounds.size.x / 2) - (textBounds.size.x / 2),
                                       boxBounds.position.y + 35.f});
    }


    loadBankData();
    initGoldDisplay();
    updateShopInfoBoxes();

}

bool drawShop(RenderWindow &window)
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

    window.draw(shopBG);
    
    for (int i = 0; i < 5; i++)
    {
        if (shopUpgrades[i].hovering && !towerMaxed[i])
        {
            if (shopUpgrades[i].visual.getPosition().y >= shopUpgrades[i].position.y - 5.f)
            {
                if (shopUpgrades[i].moveClock.getElapsedTime().asMilliseconds() >= 15)
                {
                    shopUpgrades[i].visual.move({0, -1});
                    shopUpgrades[i].moveClock.restart();
                }
            }
        }

        window.draw(shopItems[i]);
        window.draw(shopDisplayTowers[i]);
        window.draw(shopUpgrades[i].visual);
        window.draw(shopInfoBoxes[i]);
        window.draw(shopLevelTexts[i]);
        window.draw(shopPriceTexts[i]);
    }
    window.draw(goldBox);
    window.draw(goldText);
    window.draw(backToMenu.visual);
    return stopShopMenu;
}

void handleShopEvents(const Event &ev)
{

    // checkMove(shopInfoBoxes[0],ev);  
    if (const auto move = ev.getIf<Event::MouseMoved>())
    {
        Vector2f pos = {(float)move->position.x, (float)move->position.y};
        backToMenu.hovering = backToMenu.base.getGlobalBounds().contains(pos);
        if (!backToMenu.hovering)
            backToMenu.visual.setPosition(backToMenu.base.getPosition());

        for (int i = 0; i < 5; i++)
        {
            if (shopUpgrades[i].base.getGlobalBounds().contains(pos))
            {
                shopUpgrades[i].hovering = true;
            }
            else
            {
                shopUpgrades[i].hovering = false;
                shopUpgrades[i].visual.setPosition(shopUpgrades[i].base.getPosition());
            }
        }
    }
    if (const auto click = ev.getIf<Event::MouseButtonPressed>())
    {
        Vector2f pos = {(float)(click->position).x, (float)(click->position).y};
        for (int i = 0; i < 5; i++)
        {
            if (shopUpgrades[i].base.getGlobalBounds().contains(pos))
            {
                handleUpgrade(i);
                break;
            }
        }
        if (backToMenu.base.getGlobalBounds().contains(pos))
        {
            if (stopMenu && !stopShopMenu)
            {

                backToMenu.hovering = false;
                shopEvents.pause();
                stopMenu = false;
                stopShopMenu = true;
                queue.pushBack(displayMenu);
                mainMenuClicks.resume();
            }
        }
    }
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
            awaitTransit.restart();
            mapClicks.addEvent(handleMapEvents);
            mapClicks.resume();
            stopMap = false;
            break;
        case 4:
            gameWindow->close();
            break;
        case 2:
            loadShop();
            stopMenu = true;
            stopShopMenu = false;
            mainMenuClicks.pause();
            shopEvents.resume();
            queue.pushBack(drawShop);
            break;
        case 1:
        loadInstructions();
        stopMenu = true;
        mainMenuClicks.pause();
        instructionEvents.resume();
        queue.pushBack(drawInstructions);
        stopInstructions = false;
        break;

        case 3:
        loadSettings();
        stopMenu = true;
        stopSettings = false;
        mainMenuClicks.pause();
        queue.pushBack(drawSettings);
        settingsEvents.resume();
        }
    }
}

bool displayMenu(RenderWindow &window)
{
    window.draw(mainMenu);
    return stopMenu;
}

Texture map("./Assets/Map-Items/map_0.png");
Sprite mapSprite(map);
RectangleShape mapHitBoxes[3];

void loadHomeButton()
{
    float r = 40.f;
    mapSprite.setScale({0.4166f, 0.4755f});
    backToMenu.rad = r;
    backToMenu.base.setRadius(r);
    backToMenu.visual.setRadius(r);
    backToMenu.base.setOrigin({r, r});
    backToMenu.visual.setOrigin({r, r});
    backToMenu.visual.setTexture(&homeButtonTexture);
    backToMenu.visual.setTextureRect({{240, 57}, {226, 243}});

    backToMenu.base.setPosition({1200 - r - 20, 700 - r - 20});
    backToMenu.visual.setPosition(backToMenu.base.getPosition());
    backToMenu.position = backToMenu.base.getPosition();
}

RectangleShape mapBG({1200,700});
Texture mapBG_texture("./Assets/Map-Items/map_0.jpeg");
Texture leftButton_texture("./Assets/Map-Items/buttonLeft.png");
Texture rightButton_texture("./Assets/Map-Items/buttonRight.png");

void loadMap()
{
    mapBG.setTexture(&mapBG_texture);
    mapBG.setFillColor(Color(255,255,255,50));
    loadHomeButton();
    loadHomeButton();
    mapSelector = MapCarousel();
    mapSelector.addMap(mapThumbnails[0], 1);
    mapSelector.addMap(mapThumbnails[1], 2);
    mapSelector.addMap(mapThumbnails[2], 3);
    mapSelector.setButtonTextures(&leftButton_texture , &rightButton_texture);
}


void handleMapEvents(const Event &ev) {
    if(awaitTransit.getElapsedTime().asMilliseconds() <=500) return;
    if (const auto click = ev.getIf<Event::MouseButtonPressed>()) {
        Vector2f pos = {(float)click->position.x, (float)click->position.y};
        if (backToMenu.base.getGlobalBounds().contains(pos)) {
            mapClicks.pause();
            stopMap = true;
            stopMenu = false;
            queue.pushBack(displayMenu);
            mainMenuClicks.resume();
            return;
        }
        mapSelector.handleInput(pos);
        int selected = mapSelector.getSelectedLevel(pos);
        if (selected != -1) {
            mapClicks.pause();
            stopMap = true;
            tempGold = currentGold;
            loadGrid(selected);
            queue.pushBack(drawGrid);
            if(selected == 1 && currentGold < 400){
                currentGold = 400;
            }
            else if(selected == 2 && currentGold < 650){
                currentGold = 650;
            }
            else if(selected == 3 && currentGold < 900){
                currentGold = 900;
            }
            loadLevel(selected);
            saveBankData();
            queue.pushBack(drawLevel);
            inGameEvents.resume();
        }
    }
    if (const auto hover = ev.getIf<Event::MouseMoved>()){
        Vector2f pos = {(float)hover->position.x, (float)hover->position.y};
        mapSelector.handleHover(pos);
    }
}

bool drawMap(RenderWindow &window) {

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
    
    window.draw(mapBG);
    mapSelector.update();
    mapSelector.draw(window);
    
    window.draw(backToMenu.visual);
    return stopMap;
}
