#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "../Utility-Classes/displayQueue.h"

int currentGold = 0;
int towerLevels[5] = {1, 1, 1, 1, 1};
int towerPrices[5][5] = {0};
bool towerMaxed[5] = {false, false, false, false, false};

extern Font infoFont;
extern CircularButton shopUpgrades[5];

RectangleShape goldBox;
Texture goldBox_Texture("./Assets/goldDisplay.png");
Text goldText(infoFont);
string priceStrings[5];

RectangleShape shopInfoBoxes[5];
Text shopLevelTexts[5] = {Text(infoFont), Text(infoFont), Text(infoFont), Text(infoFont), Text(infoFont)};
Text shopPriceTexts[5] = {Text(infoFont), Text(infoFont), Text(infoFont), Text(infoFont), Text(infoFont)};

void loadBankData();
void updateShopInfoBoxes();
void updateGoldDisplay();
void saveBankData();

void loadBankData()
{
    ifstream bankFile("./Data-Files/bank.txt");
    if (!bankFile.is_open())
    {
        cerr << "ERROR : Failed to open bank.txt! Using default values.\n";
        currentGold = 500;
        int defaultPrices[5][5] = {
            {100, 200, 300, 400, 500},
            {150, 250, 350, 450, 550},
            {200, 300, 400, 500, 600},
            {250, 350, 450, 550, 650},
            {300, 400, 500, 600, 700}};
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
                towerPrices[i][j] = defaultPrices[i][j];
        return;
    }

    ifstream levelFile("./Data-Files/towerLevels.txt");
    if (levelFile.is_open())
    {
        for (int i = 0; i < 5; i++)
        {
            string line;
            if (getline(levelFile, line))
            {
                towerLevels[i] = stoi(line);
                if (towerLevels[i] >= 5)
                    towerMaxed[i] = true;
            }
        }
        levelFile.close();
        cerr << "MANAGER : Tower levels loaded.\n";
    }
    else
    {
        cerr << "BRUH LEVEL FILE NOT LOADED >>>> FFS...\n";
    }

    string line;
    getline(bankFile, line);
    currentGold = stoi(line);

    for (int i = 0; i < 5; i++)
    {
        if (!getline(bankFile, line))
            break;

        stringstream ss(line);
        string price;
        int levelIndex = 0;
        while (getline(ss, price, ',') && levelIndex < 5)
        {
            towerPrices[i][levelIndex] = stoi(price);
            levelIndex++;
        }
    }

    bankFile.close();
}

void saveBankData()
{
    ofstream bankFile("./Data-Files/bank.txt");
    if (!bankFile.is_open())
    {
        cerr << "ERROR : Failed to open bank.txt for saving data!\n";
        return;
    }
    bankFile << currentGold << "\n";
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            bankFile << towerPrices[i][j];
            if (j < 4)
                bankFile << ",";
        }
        bankFile << "\n";
    }

    bankFile.close();
    ofstream levelFile("./Data-Files/towerLevels.txt");
    if (levelFile.is_open())
    {
        for (int i = 0; i < 5; i++)
        {
            levelFile << towerLevels[i] << "\n";
        }
        levelFile.close();
    }
    else
    {
        cerr << "ERROR : Failed to Open towerLevels.txt!\n";
    }
}


void handleUpgrade(int towerIndex)
{
    if (towerMaxed[towerIndex])
    {
        return;
    }

    int currentLevel = towerLevels[towerIndex]; // 1-5
    int nextLevel = currentLevel + 1;

    if (nextLevel > 5)
    {
        return;
    }

    int price = towerPrices[towerIndex][nextLevel - 1];

    if (currentGold >= price)
    {
        currentGold -= price;
        updateGoldDisplay();

        towerLevels[towerIndex] = nextLevel;

        if (nextLevel == 5)
        {
            towerMaxed[towerIndex] = true;
        }
        else
        {
            // cerr << "Tower " << towerIndex << " upgraded to level " << nextLevel << "!\n";
        }

        updateShopInfoBoxes();
        saveBankData();
    }
    else
    {
        // cerr << "Not enough gold! Need " << price << " G, have " << currentGold << " G\n";
    }
}
void initGoldDisplay()
{
    goldBox.setSize({455.f, 132.f});
    goldBox.setPosition({4.f, 4.f});
    goldBox.setTexture(&goldBox_Texture);
    goldBox.setTextureRect({{111,118}, {455,132}});
    goldBox.setScale({150.f/455.f , 40.f/132.f});

    goldText.setFont(infoFont);
    goldText.setString(to_string(currentGold));
    goldText.setCharacterSize(27);
    goldText.setFillColor(Color(255, 215, 0));
    // goldText.setStyle(Text::Bold);
    goldText.setOutlineColor(Color::Black);
    goldText.setOutlineThickness(1.f);
    goldText.setPosition({60.f , 3.f});
}

void updateGoldDisplay()
{
    goldText.setString(to_string(currentGold));
    goldText.setPosition({60.f , 3.f});

}

void updateShopInfoBoxes()
{
    for (int i = 0; i < 5; i++)
    {
        if (towerMaxed[i])
        {
            shopLevelTexts[i].setString("MAX");
            shopPriceTexts[i].setString("");
            shopUpgrades[i].base.setFillColor(Color(100, 100, 100, 100));
            shopUpgrades[i].visual.setFillColor(Color(100, 100, 100, 200));
        }
        else
        {
            int currentLevel = towerLevels[i];
            shopLevelTexts[i].setString("Level " + to_string(currentLevel));

            if (currentLevel < 5)
            {
                int nextPrice = towerPrices[i][currentLevel]; // currentLevel is 1-4, index currentLevel (0-3)
                shopPriceTexts[i].setString(to_string(nextPrice) + " G");
            }
            else
            {
                shopPriceTexts[i].setString("MAX");
            }
        }
        FloatRect boxBounds = shopInfoBoxes[i].getGlobalBounds();
        FloatRect textBounds = shopPriceTexts[i].getLocalBounds();
        shopPriceTexts[i].setPosition({boxBounds.position.x + (boxBounds.size.x / 2) - (textBounds.size.x / 2),
                                       boxBounds.position.y + 35.f});
    }
}
