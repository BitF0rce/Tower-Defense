#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Plot.h"
#include "Tower.h"
#include "masterFile.h"
#include <ctime>
using namespace std;
using namespace sf;

extern Plot *grid[rows][cols];
Texture monster_01("./Assets/Monster_1.png");
Texture monster_02("./Assets/Monster_2.png");
Texture testGoblin("./Assets/KnightBasic/Run/Knight_Run_dir7.png");

class Enemy : public Entity
{
protected:
    CircleShape tracerDot;
    RectangleShape healthBarBase;
    RectangleShape healthBarStatus;
    Vector2i lastTile;
    Vector2f position;
    int strengthLevel;
    float attackDelay; // ms
    RectangleShape characterBase, hitBox;
    int currentFrame;
    float moveSpeed = 2.0f;
    float originalMoveSpeed = 2.0f;
    Clock freezeTimer;
    bool isFrozen = false;
    float freezeDuration = 1000.f;

public:
    Clock clk;
    friend class TypedDisplayQueue<Bullet, Enemy>;
    Enemy()
    {
        hitBox.setSize({30.0f, 45.0f});
        hitBox.setOrigin({15.f, 35.5f});
        hitBox.setFillColor(Color(255, 255, 255, 200));
        lastTile = {-1, -1};
        position = {4.0f, 90.0f};
        strengthLevel = 1;
        attackDelay = 1400;
        healthBarBase.setSize({30.f, 5.f});
        healthBarBase.setFillColor(Color::Black);
        healthBarStatus.setSize({30.f, 5.f});
        healthBarStatus.setFillColor(Color::Green);
        healthBarBase.setPosition({position.x, position.y - 40});
        healthBarStatus.setPosition({position.x, position.y - 40.f});
        // characterBase.setOutlineColor(Color::Black);
        // characterBase.setOutlineThickness(2);
    }
    bool getHealth()
    {
        return health;
    }
    void updatePlayerHealth(){
        if(playerHealth <= 0){
            playerHealth = 0;
            gameOver = true;
        }
        for(int i=0;i<5;i++){
            if(i<playerHealth){
                hearts[i].setTextureRect({{21,26} , {294,290}});
            }
            else{
                 hearts[i].setTextureRect({{365,26} , {294,290}});
            }
        }
    }
    void updateAttachedUI()
    {
        Vector2f pos = characterBase.getPosition();
        healthBarBase.setPosition({pos.x - 5, pos.y - 40.f});
        healthBarStatus.setPosition({pos.x - 5, pos.y - 40.f});
        hitBox.setPosition(characterBase.getPosition());
        tracerDot.setPosition({position.x, position.y - 10.f});
    }

    virtual void moveDown()
    {
        characterBase.move({moveSpeed, moveSpeed / 2.0f});
        updateAttachedUI();
        position = hitBox.getPosition();
    }
    virtual void moveRight()
    {
        characterBase.move({moveSpeed, -moveSpeed / 2.0f});
        updateAttachedUI();
        position = hitBox.getPosition();
    }
    virtual void moveLeft()
    {
        characterBase.move({-moveSpeed, moveSpeed / 2.0f});
        updateAttachedUI();
        position = hitBox.getPosition();
    }
    virtual void moveUp()
    {
        characterBase.move({-moveSpeed, -moveSpeed / 2.0f});
        updateAttachedUI();
        position = hitBox.getPosition();
    }
    virtual void attack() = 0;
    virtual void collide(float healthImpact)
    {
        health = max(health - healthImpact, 0.f);
        float newRatio = health / maxHealth;
        healthBarStatus.setSize({healthBarBase.getSize().x * newRatio, 5.f});
        if(health<=0){
            GOLD+=20;
        }
    }
    virtual Vector2f getVelocity()
    {
        auto [row, col] = screenToTile(position.x, position.y);
        int dir = grid[row][col]->getDirection();

        switch (dir)
        {
        case 1:
            return {moveSpeed, moveSpeed / 2.0f};
        case 2:
            return {moveSpeed, -moveSpeed / 2.0f};
        case 3:
            return {-moveSpeed, moveSpeed / 2.0f};
        default:
            return {0, 0};
        }
    }
    void applyFreeze()
    {
        if (!isFrozen)
        {
            originalMoveSpeed = moveSpeed;
            moveSpeed *= 0.4f;
            isFrozen = true;
            characterBase.setFillColor(Color(100, 150, 255));
        }
        freezeTimer.restart();
    }
    void updateStatus()
    {
        if (isFrozen && freezeTimer.getElapsedTime().asMilliseconds() >= freezeDuration)
        {
            moveSpeed = originalMoveSpeed;
            isFrozen = false;
            characterBase.setFillColor(Color::White);
        }
    }
};

class Goblin : public Enemy
{
public:
    Goblin()
    {
        characterBase.setTexture(&testGoblin);
        characterBase.setSize({78.f, 106.f});
        characterBase.setOrigin({39.f, 53.f});
        characterBase.setPosition(position);
        health = 700;
        maxHealth = 700;
        moveSpeed = 1;
        tracerDot.setRadius(2);
    }
    void attack()
    {
    }
    void draw(RenderWindow &window)
    {
        if (health <= 0)
            return;
        if (clk.getElapsedTime().asMilliseconds() >= 20)
        {
            clk.restart();
            currentFrame = (currentFrame + 1) % 8;

            int r = currentFrame / 3;
            int c = currentFrame % 3;

            characterBase.setTextureRect(IntRect(
                {c * 256, r * 256},
                {256, 256}));
            Vector2f pos = characterBase.getPosition();
            auto [row, col] = screenToTile(pos.x, pos.y - 10);
            Vector2i currentTile = {row, col};
            if (currentTile != lastTile)
            {
                masterManager.poll(this, currentTile, {position.x, position.y - 30.f}, getVelocity());
            }
            int dir = grid[row][col]->getDirection();
            if(dir<=0){
                cerr<<"Enemy Reached the destination.... Killing now.\n";
                health = 0;
                playerHealth--;
                updatePlayerHealth();

            }
            switch (dir)
            {
            case 0:
                break;
            case 1:
                moveDown();
                break;
            case 2:
                moveRight();
                break;
            case 3:
                moveLeft();
            default:
                break;
            }
        }
        updateStatus();
        window.draw(characterBase);
        window.draw(healthBarBase);
        window.draw(healthBarStatus);
    }
};

class FlashDude : public Enemy
{
    Clock speedAlternator;

public:
    FlashDude()
    {
        characterBase.setTexture(&testGoblin);
        characterBase.setSize({78.f, 106.f});
        characterBase.setOrigin({39.f, 53.f});
        characterBase.setPosition(position);
        health = 500;
        maxHealth = 600;
        moveSpeed = 3.0f;
        tracerDot.setRadius(2);
    }
    void attack()
    {
    }
    void draw(RenderWindow &window)
    {
        if (health <= 0)
            return;
        if (moveSpeed == 3 && !isFrozen)
        {
            if (rand() % 25 == 0)
            {
                moveSpeed = 5;
                speedAlternator.restart();
            }
        }
        else if (moveSpeed <= 5 && !isFrozen)
        {
            if (speedAlternator.getElapsedTime().asMilliseconds() >= 500 || moveSpeed <= 3)
            {
                moveSpeed = 3;
                // cerr<<"Back to Normal...\n";
            }
            else
            {
                moveSpeed -= 0.005;
            }
        }
        if (clk.getElapsedTime().asMilliseconds() >= ((moveSpeed > 2.5f) ? 20 : 15))
        {
            clk.restart();
            currentFrame = (currentFrame + 1) % 8;

            int r = currentFrame / 3;
            int c = currentFrame % 3;

            characterBase.setTextureRect(IntRect(
                {c * 256, r * 256},
                {256, 256}));
            Vector2f pos = characterBase.getPosition();
            auto [row, col] = screenToTile(pos.x, pos.y - 10);
            Vector2i currentTile = {row, col};
            if (currentTile != lastTile)
            {
                masterManager.poll(this, currentTile, {position.x, position.y - 30.f}, getVelocity());
            }
            int dir = grid[row][col]->getDirection();
            if(dir<=0){
                cerr<<"Enemy Reached the destination.... Killing now.\n";
                health = 0;
                playerHealth--;
                updatePlayerHealth();
            }
            switch (dir)
            {
            case 0:
                break;
            case 1:
                moveDown();
                break;
            case 2:
                moveRight();
                break;
            case 3:
                moveLeft();
            default:
                break;
            }
        }
        updateStatus();
        window.draw(characterBase);
        window.draw(healthBarBase);
        window.draw(healthBarStatus);
        window.draw(tracerDot);
    }
};

class TankDude : public Enemy
{
public:
    TankDude()
    {
        characterBase.setTexture(&testGoblin);
        characterBase.setSize({78.f, 106.f});
        characterBase.setOrigin({39.f, 53.f});
        characterBase.setPosition(position);
        health = 700;
        maxHealth = 700;
        moveSpeed = 1;
        tracerDot.setRadius(2);
    }
    void attack()
    {
    }
    void draw(RenderWindow &window)
    {
        if (health <= 0)
            return;
        if (clk.getElapsedTime().asMilliseconds() >= 20)
        {
            clk.restart();
            currentFrame = (currentFrame + 1) % 8;

            int r = currentFrame / 3;
            int c = currentFrame % 3;

            characterBase.setTextureRect(IntRect(
                {c * 256, r * 256},
                {256, 256}));
            Vector2f pos = characterBase.getPosition();
            auto [row, col] = screenToTile(pos.x, pos.y - 10);
            Vector2i currentTile = {row, col};
            if (currentTile != lastTile)
            {
                masterManager.poll(this, currentTile, {position.x, position.y - 30.f}, getVelocity());
            }
            int dir = grid[row][col]->getDirection();
            if(dir<=0){
                cerr<<"Enemy Reached the destination.... Killing now.\n";
                health = 0;
                playerHealth--;
                updatePlayerHealth();
            }
            switch (dir)
            {
            case 0:
                break;
            case 1:
                moveDown();
                break;
            case 2:
                moveRight();
                break;
            case 3:
                moveLeft();
            default:
                break;
            }
        }
        updateStatus();
        window.draw(characterBase);
        window.draw(healthBarBase);
        window.draw(healthBarStatus);
        window.draw(tracerDot);
    }
};

class Jahaaz : public Enemy
{
    Vector2f destination, unitVector;

public:
    Jahaaz()
    {

        characterBase.setTexture(&testGoblin);
        characterBase.setSize({78.f, 106.f});
        characterBase.setOrigin({39.f, 53.f});
        characterBase.setPosition(position);
        health = 700;
        maxHealth = 700;
        moveSpeed = 1;
        tracerDot.setRadius(2);
        destination = {1200.f, 350.f};
        Vector2f distance = (destination - position);
        float length = sqrt(distance.x * distance.x + distance.y * distance.y);
        unitVector = distance / length;
    }
    void attack()
    {
    }
    void moveLinearly()
    {
        characterBase.move(unitVector * moveSpeed);
        position = characterBase.getPosition();
        updateAttachedUI();
    }
    void moveUp() override { moveLinearly(); }
    void moveDown() override { moveLinearly(); }
    void moveLeft() override { moveLinearly(); }
    void moveRight() override { moveLinearly(); }

    Vector2f getVelocity() override
    {
        return unitVector * moveSpeed;
    }

    void draw(RenderWindow &window)
    {
        if (health <= 0)
            return;
        if (clk.getElapsedTime().asMilliseconds() >= 20)
        {
            clk.restart();
            currentFrame = (currentFrame + 1) % 16;
            // characterBase->setScale({0.2f, 0.2f});
            characterBase.setTextureRect({{(currentFrame * 42), 46 * 4}, {42, 46}});
            Vector2f pos = characterBase.getPosition();
            auto [row, col] = screenToTile(pos.x, pos.y);
            Vector2i currentTile = {row, col};
            if (currentTile != lastTile)
            {
                masterManager.poll(this, currentTile, {position.x - 21, position.y - 23}, getVelocity());
            }
            int dir = grid[row][col]->getDirection();
            moveLinearly();
        }
        updateStatus();
        window.draw(characterBase);
        window.draw(healthBarBase);
        window.draw(healthBarStatus);
    }
};

TankDude g;
bool drawEnemy(RenderWindow &window)
{
    g.draw(window);
    return false;
}

Enemy *enemies[20] = {};
int enemyCount = 0;

void spawnEnemy(Enemy *e)
{
    if (enemyCount < 20)
    {
        enemies[enemyCount] = e;
        enemyCount++;
    }
}

void clearEnemies()
{
    for (int i = 0; i < enemyCount; i++)
    {
        delete enemies[i];
        enemies[i] = nullptr;
    }
    enemyCount = 0;
}

bool drawEnemies(RenderWindow &window)
{
    for (int i = 0; i < enemyCount; i++)
    {
        if (enemies[i])
            enemies[i]->draw(window);
    }
    return false;
}

int currentWave = 0;
Clock spawnClock;
bool levelComplete = false;
bool waveInProgress = false; // true while spawning/fighting

struct WaveConfig
{
    int goblins;
    int tanks;
    int flashDudes;
};

WaveConfig waves[5] = {
    {0, 0, 2},
    {0, 0, 4},
    {6, 0, 0},
    {4, 1, 0},
    {4, 1, 1}};

int toSpawnGoblin = 0, toSpawnTank = 0, toSpawnFlash = 0;
int totalInWave = 0;
int spawnedSoFar = 0;

bool allDead()
{
    if (enemyCount == 0)
        return false; // nothing spawned yet
    for (int i = 0; i < enemyCount; i++)
        if (enemies[i] && enemies[i]->getHealth() > 0)
            return false;
    return true;
}

void startWave(int waveIndex)
{

    char x = currentWave+48;
    char str[] = "Wave : 1/5";
    str[7] = x;
    waveInfo.setString(str);
    clearEnemies();
    toSpawnGoblin = waves[waveIndex].goblins;
    toSpawnTank = waves[waveIndex].tanks;
    toSpawnFlash = waves[waveIndex].flashDudes;
    totalInWave = toSpawnGoblin + toSpawnTank + toSpawnFlash;
    spawnedSoFar = 0;
    waveInProgress = true;
    waitingForSpawn = false;
    spawnClock.restart();
    // pause UI interactions during wave
    towerMenuEvents.pause();
    anchorHover.pause();
    spawnButton.setFillColor(Color(0, 0, 0, 100)); // start faded
    cerr << "Wave " << waveIndex + 1 << " starting\n";
}

void updateWaves()
{
    if (!waveInProgress)
        return;

    // spawn with delay
    bool doneSpawning = (toSpawnGoblin == 0 && toSpawnTank == 0 && toSpawnFlash == 0);

    if (!doneSpawning && spawnClock.getElapsedTime().asMilliseconds() >= 1500)
    {
        spawnClock.restart();
        if (toSpawnGoblin > 0)
        {
            spawnEnemy(new Goblin());
            toSpawnGoblin--;
        }
        else if (toSpawnTank > 0)
        {
            spawnEnemy(new TankDude());
            toSpawnTank--;
        }
        else if (toSpawnFlash > 0)
        {
            spawnEnemy(new FlashDude());
            toSpawnFlash--;
        }
        spawnedSoFar++;
    }

    // gradual button color — progresses as enemies die
    int deadCount = 0;
    for (int i = 0; i < enemyCount; i++)
        if (enemies[i] && enemies[i]->getHealth() <= 0)
            deadCount++;

    float progress = (enemyCount > 0) ? (float)deadCount / totalInWave : 0.f;
    uint8_t brightness = (uint8_t)(progress * 255);
    spawnButton.setFillColor(Color(brightness, brightness, brightness));

    // wave over
    if (doneSpawning && allDead())
    {
        waveInProgress = false;
        waitingForSpawn = true;
        spawnButton.setFillColor(Color::White); // fully lit — ready
        // resume UI
        towerMenuEvents.resume();
        anchorHover.resume();
        cerr << "Wave " << currentWave + 1 << " complete — press spawn for next\n";
    }
}


void onSpawnButtonClick()
{
    if (!waitingForSpawn || levelComplete)
        return;
    if (currentWave < 4)
    {
        currentWave++;
        startWave(currentWave);
    }
    else
    {
        levelComplete = true;
        cerr << "Level Complete!\n";
    }
}

void loadLevel1()
{
    currentWave = 0;
    levelComplete = false;
    waveInProgress = false;
    waitingForSpawn = true;
    clearEnemies();
    spawnButton.setFillColor(Color::White); // ready for first wave click
    towerMenuEvents.resume();
    anchorHover.resume();
}

bool drawLevel1(RenderWindow &window)
{
    updateWaves();
    drawEnemies(window);
    return levelComplete;
}
