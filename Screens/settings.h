#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include "../Utility-Classes/displayQueue.h"
#include "shop.h"

using namespace sf;
using namespace std;

extern displayQueue queue;
extern EventHandler mainMenuClicks;
extern EventHandler settingsEvents;
extern Texture overlayButton_texture;
extern CircularButton backToMenu;
extern bool stopMenu;
extern Font infoFont;
RectangleShape settingsBG({ 1200.f, 700.f });
RectangleShape resetBtn;
Text resetText(infoFont);
bool stopSettings = false;

extern Texture mapBG_texture;


bool displayMenu(RenderWindow& window);
void loadHomeButton();
void handleSettingsEvents(const Event& ev) {
    if (const auto move = ev.getIf<Event::MouseMoved>()) {
        Vector2f pos = { (float)move->position.x, (float)move->position.y };
        backToMenu.hovering = backToMenu.base.getGlobalBounds().contains(pos);
        if (!backToMenu.hovering)
            backToMenu.visual.setPosition(backToMenu.base.getPosition());
        
        if (resetBtn.getGlobalBounds().contains(pos)){
            resetBtn.setTextureRect({{346, 139}, {222, 127}});
        }
        else{
            resetBtn.setTextureRect({{42, 144}, {219, 117}});
        }
    }

    if (const auto click = ev.getIf<Event::MouseButtonPressed>()) {
        Vector2f pos = { (float)click->position.x, (float)click->position.y };
        
        if (backToMenu.base.getGlobalBounds().contains(pos)) {
            settingsEvents.pause();
            stopSettings = true;
            stopMenu = false;
            queue.pushBack(displayMenu);
            mainMenuClicks.resume();
        }

        if (resetBtn.getGlobalBounds().contains(pos)) {
            currentGold = 500;
            for(int i=0; i<5; i++) towerLevels[i] = 1;
            saveBankData();
            loadBankData();
        }
    }
}

void loadSettings() {
    loadHomeButton();

    settingsBG.setTexture(&mapBG_texture);
    settingsBG.setFillColor(Color(255,255,255,50));


    settingsEvents.addEvent(handleSettingsEvents);

    resetBtn.setSize({300.f, 110.f});
    resetBtn.setOrigin({150.f, 40.f});
    resetBtn.setPosition({600.f, 350.f});
    resetBtn.setTexture(&overlayButton_texture);
    resetBtn.setTextureRect({{42, 144}, {219, 117}});

    resetText.setFont(infoFont);
    resetText.setString("FACTORY RESET");
    resetText.setCharacterSize(30);
    resetText.setFillColor(Color::White);
    // Center text in button
    FloatRect textBounds = resetText.getLocalBounds();
    resetText.setOrigin({textBounds.size.x / 2, textBounds.size.y / 2});
    resetText.setPosition(resetBtn.getPosition());
}

bool drawSettings(RenderWindow& window) {
    if (backToMenu.hovering) {
        if (backToMenu.visual.getPosition().y >= backToMenu.position.y - 5.f) {
            if (backToMenu.moveClock.getElapsedTime().asMilliseconds() >= 10) {
                backToMenu.visual.move({ 0, -1 });
                backToMenu.moveClock.restart();
            }
        }
    }

    window.draw(settingsBG);
    window.draw(resetBtn);
    window.draw(resetText);
    window.draw(backToMenu.visual);
    return stopSettings;
}