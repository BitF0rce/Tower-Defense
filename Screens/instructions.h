#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "../Utility-Classes/displayQueue.h"

using namespace sf;
using namespace std;

extern RenderWindow* gameWindow;
extern displayQueue queue;
extern EventHandler mainMenuClicks;
extern Font infoFont;
extern CircularButton backToMenu;
extern bool stopMenu;

void loadHomeButton();
bool displayMenu(RenderWindow &window);

extern EventHandler instructionEvents;

RectangleShape instructionBG({ 1200.f, 700.f });
RectangleShape towerCards[5];
Text towerInfo[5] = {Text(infoFont), Text(infoFont),Text(infoFont),Text(infoFont),Text(infoFont)};
RectangleShape enemyCards[5];
Text enemyInfo[5] = {Text(infoFont), Text(infoFont),Text(infoFont),Text(infoFont),Text(infoFont)};


RectangleShape callWaveRect;
Text callWaveDesc(infoFont);
Texture instructions_towers_texture("./Assets/TowerMenuIcons.png");
extern Texture callEnemy;
Texture instructions_enemy[5] = {Texture("./Assets/Goblin/left.png") , Texture("./Assets/Tank/left.png") , Texture("./Assets/Flash/left.png") , Texture("./Assets/Tank/left.png") , Texture("./Assets/Wizard/left.png")};
bool stopInstructions = false;

void handleInstructionEvents(const Event& ev) {
    if (const auto move = ev.getIf<Event::MouseMoved>()) {
        Vector2f pos = { (float)move->position.x, (float)move->position.y };
        backToMenu.hovering = backToMenu.base.getGlobalBounds().contains(pos);
        if (!backToMenu.hovering)
            backToMenu.visual.setPosition(backToMenu.base.getPosition());
    }

    if (const auto click = ev.getIf<Event::MouseButtonPressed>()) {
        Vector2f pos = { (float)click->position.x, (float)click->position.y };
        if (backToMenu.base.getGlobalBounds().contains(pos)) {
            instructionEvents.pause();
            stopInstructions = true;
            stopMenu = false;
            queue.pushBack(displayMenu);
            mainMenuClicks.resume();
        }
    }
}

void loadInstructions() {
    loadHomeButton();
    instructionEvents.addEvent(handleInstructionEvents);
    instructionBG.setFillColor(Color(245, 235, 220));

    float startX = 100.f;
    float startY = 80.f;
    float colWidth = 500.f;
    float rowHeight = 90.f;

    for (int i = 0; i < 5; i++) {
        towerCards[i].setSize({ 60.f, 60.f });
        towerCards[i].setPosition({ startX, startY + (i * rowHeight) });
        towerCards[i].setTexture(&instructions_towers_texture);
        towerCards[i].setTextureRect({{17 + (i * 133), 109}, {133, 135}});


        towerInfo[i].setFont(infoFont); // Reuses font from Plot.h/Shop.h
        towerInfo[i].setCharacterSize(18);
        towerInfo[i].setFillColor(Color::Black);
        towerInfo[i].setString("Tower Type " + to_string(i + 1) + "\nRange: ... Damage: ...");
        towerInfo[i].setPosition({ startX + 80.f, startY + (i * rowHeight) + 10.f });

        enemyCards[i].setSize({ 60.f, 60.f });
        enemyCards[i].setPosition({ startX + colWidth, startY + (i * rowHeight) });
        enemyCards[i].setTexture(&instructions_enemy[i]);
        enemyCards[i].setTextureRect({{0,0} , {68,68}});

        enemyInfo[i].setFont(infoFont);
        enemyInfo[i].setCharacterSize(18);
        enemyInfo[i].setFillColor(Color::Black);
        enemyInfo[i].setString("Enemy Type " + to_string(i + 1) + "\nHealth: ... Reward: ...");
        enemyInfo[i].setPosition({ startX + colWidth + 80.f, startY + (i * rowHeight) + 10.f });
    }


    enemyInfo[0].setString("Goblin\nBasic Stats : Moderate Health , Moderate Speed");
    enemyInfo[1].setString("Tank\nHealth Oriented : High Health , Nerfed Speed");
    enemyInfo[2].setString("Flash\nSpeed Oriented  : Low Health , High and Alternating Speed\nlevels to trick towers.");
    enemyInfo[3].setString("Jahaaz\nMovement Oriented : Moderate Health , Linear Movement.\nFollows no path tiles.");
    enemyInfo[4].setString("Wizard\nIllusionier : Applies Magic to vanish off of Towers\nRadar for a while");

    towerInfo[0].setString("Sniper\nLong Range with High Precisions, Average Attack\nDamage");
    towerInfo[1].setString("Machine Gun\nMid Range with the Ability to take on Swarms\nat once");
    towerInfo[2].setString("Cannon\nLow Firing Rate, Insanely High Attack Damage");
    towerInfo[3].setString("Slow\nLong Range Potion Effects : Slows down the\n Enemies. Better if used in a combination");
    towerInfo[4].setString("ShotGun\nFires 3 Bullets at once, Covering a wider Angle");

    callWaveRect.setSize({ 70.f, 70.f });
    callWaveRect.setPosition({ 100.f, 560.f });
    callWaveRect.setTexture(&callEnemy);
    callWaveRect.setTextureRect({{238, 60}, {232, 238}});

    callWaveDesc.setFont(infoFont);
    callWaveDesc.setCharacterSize(20);
    callWaveDesc.setFillColor(Color::Black);
    callWaveDesc.setString("CALL WAVE BUTTON:\nPress to manually start the next wave. \nVanished after the wave has started. Gets enabled back after the called wave is over.");
    callWaveDesc.setPosition({ 200.f, 570.f });
}

bool drawInstructions(RenderWindow& window) {
    if (backToMenu.hovering) {
        if (backToMenu.visual.getPosition().y >= backToMenu.position.y - 5.f) {
            if (backToMenu.moveClock.getElapsedTime().asMilliseconds() >= 10) {
                backToMenu.visual.move({ 0, -1 });
                backToMenu.moveClock.restart();
            }
        }
    }

    window.draw(instructionBG);
    for (int i = 0; i < 5; i++) {
        window.draw(towerCards[i]);
        window.draw(towerInfo[i]);
        window.draw(enemyCards[i]);
        window.draw(enemyInfo[i]);
    }

    window.draw(callWaveRect);
    window.draw(callWaveDesc);
    window.draw(backToMenu.visual);

    return stopInstructions;
}