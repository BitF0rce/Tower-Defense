#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include "Plot.h"
#include "Tower.h"
#include "masterFile.h"
#include "../Screens/main.h"
#include "../Screens/shop.h"
using namespace std;
using namespace sf;

extern Plot *grid[rows][cols];
extern float playerHealth;
extern int currentGold;
extern bool gamePaused;

Texture monster_01("./Assets/Monster_1.png");
Texture monster_02("./Assets/Monster_2.png");
Texture testGoblin("./Assets/KnightBasic/Run/Knight_Run_dir7.png");

// ── Spawn / end tile per level (loaded from points.txt) ────────────────────
Vector2i spawnTile[3] = {{0, 0}, {0, 0}, {0, 0}};
Vector2i endZoneMin[3] = {{0, 0}, {0, 0}, {0, 0}};
Vector2i endZoneMax[3] = {{0, 0}, {0, 0}, {0, 0}};

int activeLevel = 0; // set by loadGrid(level)

void loadPoints()
{
    ifstream f("./Data-Files/points.txt");
    for (int lvl = 0; lvl < 3 && f; lvl++)
    {
        string line;
        getline(f, line);
        int si, sj, r1c1, r1c2, r2c1, r2c2;
        if (sscanf(line.c_str(), "%d,%d_%d,%d,%d,%d",
                   &si, &sj, &r1c1, &r1c2, &r2c1, &r2c2) == 6)
        {
            spawnTile[lvl] = {si, sj};
            endZoneMin[lvl] = {r1c1, r1c2};
            endZoneMax[lvl] = {r2c1, r2c2};
        }
    }
    cerr << "Manager : Points loaded.\n";
}

Vector2f tileCenter(int row, int col)
{
    float sx = originX + (col - row) * (tileW / 2.f) + tileW / 2.f;
    float sy = originY + (col + row) * (tileH / 2.f) + tileH / 2.f;
    return {sx, sy};
}

class Enemy : public Entity
{
protected:
    CircleShape tracerDot;
    RectangleShape healthBarBase;
    RectangleShape healthBarStatus;
    Vector2i lastTile;
    Vector2f position;
    int strengthLevel = 1;
    float attackDelay = 1400.f;
    RectangleShape characterBase, hitBox;
    int currentFrame = 0;
    float moveSpeed = 2.0f;
    float originalMoveSpeed = 2.0f;
    Clock freezeTimer;
    bool isFrozen = false;
    float freezeDuration = 1000.f;
    bool reachedEnd = false;
    int goldReward = 2;
    Vector2i pendingTile = {-1, -1};
    int pendingCount = 0;

public:
    Clock clk;
    friend class TypedDisplayQueue<Bullet, Enemy>;

    Enemy()
    {
        hitBox.setSize({30.f, 45.f});
        hitBox.setOrigin({15.f, 35.5f});
        hitBox.setFillColor(Color(255, 255, 255, 200));
        lastTile = {-1, -1};
        Vector2f sp = tileCenter(spawnTile[activeLevel].x, spawnTile[activeLevel].y);
        position = sp;
        healthBarBase.setSize({30.f, 5.f});
        healthBarBase.setFillColor(Color::Black);
        healthBarStatus.setSize({30.f, 5.f});
        healthBarStatus.setFillColor(Color::Green);
        healthBarBase.setPosition({position.x, position.y - 40.f});
        healthBarStatus.setPosition({position.x, position.y - 40.f});
    }

    float getHealth() { return health; }

    void updatePlayerHealth()
    {
        if (playerHealth < 0)
            playerHealth = 0;
        if (playerHealth == 0)
            gameOver = true;
        for (int i = 0; i < 5; i++)
        {
            if (i < playerHealth)
                hearts[i].setTextureRect({{21, 26}, {294, 290}});
            else
                hearts[i].setTextureRect({{365, 26}, {294, 290}});
        }
    }
    virtual void updateAttachedUI()
    {
        Vector2f pos = tracerDot.getPosition();
        healthBarBase.setPosition({pos.x - 5.f, pos.y - 40.f});
        healthBarStatus.setPosition({pos.x - 5.f, pos.y - 40.f});
        hitBox.setPosition(pos);
        characterBase.setPosition(pos);
    }
    virtual void moveDown()
    {
        if (gamePaused)
            return;
        tracerDot.move({moveSpeed, moveSpeed / 2.f});
        position = tracerDot.getPosition();
        updateAttachedUI();
    }
    virtual void moveRight()
    {
        if (gamePaused)
            return;
        tracerDot.move({moveSpeed, -moveSpeed / 2.f});
        position = tracerDot.getPosition();
        updateAttachedUI();
    }
    virtual void moveLeft()
    {
        if (gamePaused)
            return;
        tracerDot.move({-moveSpeed, moveSpeed / 2.f});
        position = tracerDot.getPosition();
        updateAttachedUI();
    }
    virtual void moveUp()
    {
        if (gamePaused)
            return;
        tracerDot.move({-moveSpeed, -moveSpeed / 2.f});
        position = tracerDot.getPosition();
        updateAttachedUI();
    }
    virtual void attack() = 0;

    virtual void collide(float dmg)
    {
        if (reachedEnd)
            return;
        health = max(health - dmg, 0.f);
        float ratio = health / maxHealth;
        healthBarStatus.setSize({healthBarBase.getSize().x * ratio, 5.f});
        if (health <= 0)
        {
            currentGold += goldReward;
            saveBankData();
        }
    }

    bool checkEndTile(int row, int col)
    {
        Vector2i mn = endZoneMin[activeLevel];
        Vector2i mx = endZoneMax[activeLevel];
        // cerr << "Starting : " << mn.x << "," << mn.y << endl;
        // cerr << "Ending  : " << mx.x << "," << mx.y << endl;
        // cerr << "Current Position : " << row << "," << col << endl;
        bool inZone = (row >= mn.x && row <= mx.x &&
                       col >= mn.y && col <= mx.y);

        if (!reachedEnd && inZone)
        {
            reachedEnd = true;
            health = 0;
            if (playerHealth > 0)
            {
                // cerr << "Enemy Reached Ending Tile....\n";
                playerHealth -= 0.5;
                updatePlayerHealth();
            }
            return true;
        }
        return health <= 0;
    }

    virtual Vector2f getVelocity()
    {
        auto [row, col] = screenToTile(position.x, position.y);
        int dir = grid[row][col]->getDirection();
        switch (dir)
        {
        case 1:
            return {moveSpeed, moveSpeed / 2.f};
        case 2:
            return {moveSpeed, -moveSpeed / 2.f};
        case 3:
            return {-moveSpeed, moveSpeed / 2.f};
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

    bool shouldDraw() { return health > 0 && !reachedEnd; }
    virtual void updateTexture(int a) = 0;
};

// ── Macro to reduce copy-paste in draw() ───────────────────────────────────
// Each path-following enemy has identical tile-tracking + movement code
#define PATH_ENEMY_UPDATE(yOffset)                                              \
    Vector2f pos_ = tracerDot.getPosition();                                    \
    auto [row_, col_] = screenToTile(pos_.x, pos_.y + (yOffset));               \
    Vector2i currentTile_ = {row_, col_};                                       \
    if (currentTile_ == pendingTile)                                            \
    {                                                                           \
        pendingCount++;                                                         \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        pendingTile = currentTile_;                                             \
        pendingCount = 0;                                                       \
    }                                                                           \
    if (pendingCount >= 2 && currentTile_ != lastTile)                          \
    {                                                                           \
        masterManager.poll(this, currentTile_, {position.x, position.y - 30.f}, \
                           getVelocity());                                      \
        lastTile = currentTile_;                                                \
    }                                                                           \
    if (checkEndTile(row_, col_))                                               \
        return;                                                                 \
    int dir_ = (lastTile.x >= 0) ? grid[lastTile.x][lastTile.y]->getDirection() \
                                 : grid[row_][col_]->getDirection();            \
    this->updateTexture(dir_);                                                  \
    switch (dir_)                                                               \
    {                                                                           \
    case 1:                                                                     \
        moveDown();                                                             \
        break;                                                                  \
    case 2:                                                                     \
        moveRight();                                                            \
        break;                                                                  \
    case 3:                                                                     \
        moveLeft();                                                             \
        break;                                                                  \
    case 4:                                                                     \
        moveUp();                                                               \
        break;                                                                  \
    default:                                                                    \
        break;                                                                  \
    }

Texture goblin_texture[4] = {Texture("./Assets/Goblin/down.png"), Texture("./Assets/Goblin/right.png"), Texture("./Assets/Goblin/left.png"), Texture("./Assets/Goblin/up.png")};
Texture wizard_texture[4] = {Texture("./Assets/Wizard/down.png"), Texture("./Assets/Wizard/right.png"), Texture("./Assets/Wizard/left.png"), Texture("./Assets/Wizard/up.png")};
Texture tank_texture[4] = {Texture("./Assets/Tank/down.png"), Texture("./Assets/Tank/right.png"), Texture("./Assets/Tank/left.png"), Texture("./Assets/Tank/up.png")};
Texture flash_texture[4] = {Texture("./Assets/Flash/down.png"), Texture("./Assets/Flash/right.png"), Texture("./Assets/Flash/left.png"), Texture("./Assets/Flash/up.png")};

class Goblin : public Enemy
{
public:
    Goblin()
    {
        characterBase.setTexture(&goblin_texture[0]);
        characterBase.setSize({68.f, 68.f});
        characterBase.setScale({0.6f, 0.6f});
        characterBase.setOrigin({32.f, 42.f});
        tracerDot.setPosition(position);
        characterBase.setPosition(position);
        // characterBase.setOutlineColor(Color::Black);
        // characterBase.setOutlineThickness(3);
        health = 300;
        maxHealth = 300;
        moveSpeed = 1.5f;
        goldReward = 2;
        tracerDot.setRadius(2);
    }
    void attack() {}
    void draw(RenderWindow &window)
    {
        if (!shouldDraw())
            return;
        if (clk.getElapsedTime().asMilliseconds() >= 20 && !gamePaused)
        {
            clk.restart();
            currentFrame = (currentFrame + 1) % 10;
            int r = currentFrame / 5, c = currentFrame % 5;
            if (r == 1 && c > 2)
                r = c = 0;
            characterBase.setTextureRect(IntRect({c * 68, r * 68}, {68 - 20, 68}));
            PATH_ENEMY_UPDATE(-10)
        }
        updateStatus();
        window.draw(characterBase);
        window.draw(healthBarBase);
        window.draw(healthBarStatus);
        // window.draw(hitBox);
        // window.draw(tracerDot);
    }
    void updateTexture(int a)
    {
        if (a == 1)
            characterBase.setTexture(&goblin_texture[0]);
        else if (a == 2)
            characterBase.setTexture(&goblin_texture[1]);
        else if (a == 3)
            characterBase.setTexture(&goblin_texture[2]);
        else if (a == 4)
            characterBase.setTexture(&goblin_texture[3]);
    }
};

class TankDude : public Enemy
{
    Clock spriteCOntrol;

public:
    TankDude()
    {
        characterBase.setTexture(&wizard_texture[0]);
        characterBase.setSize({68.f, 68.f});
        characterBase.setScale({0.6f, 0.6f});
        characterBase.setOrigin({32.f, 42.f});
        tracerDot.setPosition(position);
        characterBase.setPosition(position);

        health = 1200;
        maxHealth = 1200;
        moveSpeed = 0.8f;
        goldReward = 8;
        tracerDot.setRadius(2);
    }
    void attack() {}
    void draw(RenderWindow &window)
    {
        if (!shouldDraw())
            return;

        if (clk.getElapsedTime().asMilliseconds() >= 20 && !gamePaused)
        {
            clk.restart();
            PATH_ENEMY_UPDATE(-10)
        }
        if (spriteCOntrol.getElapsedTime().asMilliseconds() >= 35 && !gamePaused)
        {
            spriteCOntrol.restart();
            currentFrame = (currentFrame + 1) % 10;
            int r = currentFrame / 5, c = currentFrame % 5;
            if (r == 1 && c > 2)
                r = c = 0;
            characterBase.setTextureRect(IntRect({c * 68, r * 68}, {68 - 20, 68}));
        }
        updateStatus();
        window.draw(characterBase);
        window.draw(healthBarBase);
        window.draw(healthBarStatus);
    }
    void updateTexture(int a)
    {
        if (a == 1)
            characterBase.setTexture(&tank_texture[0]);
        else if (a == 2)
            characterBase.setTexture(&tank_texture[1]);
        else if (a == 3)
            characterBase.setTexture(&tank_texture[2]);
        else if (a == 4)
            characterBase.setTexture(&tank_texture[3]);
    }
};

class FlashDude : public Enemy
{
    Clock speedAlternator;
    float baseSpeed = 3.0f;

public:
    FlashDude()
    {
        characterBase.setTexture(&flash_texture[0]);
        characterBase.setSize({92.f, 92.f});
        characterBase.setScale({0.5f, 0.5f});
        characterBase.setOrigin({38.f, 42.f});
        tracerDot.setPosition(position);
        characterBase.setPosition(position);

        health = 500;
        maxHealth = 500;
        moveSpeed = baseSpeed;
        goldReward = 5;
        tracerDot.setRadius(2);
    }
    void attack() {}
    void draw(RenderWindow &window)
    {
        if (!shouldDraw())
            return;

        if (!isFrozen)
        {
            if (moveSpeed <= baseSpeed)
            {
                if (rand() % 300 == 0) //
                {
                    moveSpeed = 5.f;
                    speedAlternator.restart();
                }
            }
            else
            {
                if (speedAlternator.getElapsedTime().asMilliseconds() >= 500)
                    moveSpeed = baseSpeed;
                else
                    moveSpeed = max(baseSpeed, moveSpeed - 0.005f);
            }
        }

        int interval = (moveSpeed > 2.5f) ? 20 : 15;
        if (clk.getElapsedTime().asMilliseconds() >= interval && !gamePaused)
        {
            clk.restart();
            currentFrame = (currentFrame + 1) % 8;
            characterBase.setTextureRect(IntRect({currentFrame * 92, 0}, {92, 92}));
            PATH_ENEMY_UPDATE(-10)
        }
        updateStatus();
        window.draw(characterBase);
        window.draw(healthBarBase);
        window.draw(healthBarStatus);
    }
    void updateTexture(int a)
    {
        if (a == 1)
            characterBase.setTexture(&flash_texture[0]);
        else if (a == 2)
            characterBase.setTexture(&flash_texture[1]);
        else if (a == 3)
            characterBase.setTexture(&flash_texture[2]);
        else if (a == 4)
            characterBase.setTexture(&flash_texture[3]);
    }
};

Texture jahaaz_texture("./Assets/Jahaaz/up.png");

class Jahaaz : public Enemy
{
    Vector2f waypoints[3];
    int waypointIndex = 0;
    Vector2f unitVector = {0.f, 0.f};

    void computeUnitVector()
    {
        if (waypointIndex >= 3)
            return;
        Vector2f diff = waypoints[waypointIndex] - tracerDot.getPosition();
        float len = sqrt(diff.x * diff.x + diff.y * diff.y);
        if (len > 0.f)
            unitVector = diff / len;
    }

    Vector2f getEndZoneCenter()
    {
        Vector2i min = endZoneMin[activeLevel];
        Vector2i max = endZoneMax[activeLevel];
        int midRow = (min.x + max.x) / 2;
        int midCol = (min.y + max.y) / 2;
        return tileCenter(midRow, midCol);
    }

public:
    Jahaaz()
    {
        characterBase.setTexture(&jahaaz_texture);
        characterBase.setSize({92.f, 92.f});
        characterBase.setScale({0.5f, 0.5f});
        characterBase.setOrigin({38.f, 42.f});

        Vector2f start = tileCenter(spawnTile[activeLevel].x, spawnTile[activeLevel].y);
        tracerDot.setPosition(start);
        tracerDot.setRadius(2);
        position = start;
        characterBase.setPosition(start);

        health = 800;
        maxHealth = 800;
        moveSpeed = 1.8f;
        goldReward = 12;

        waypoints[0] = start;
        waypoints[1] = {600.f, 350.f};
        waypoints[2] = getEndZoneCenter();

        waypointIndex = 1;
        computeUnitVector();
    }

    void attack() override {}

    void moveLinearly()
    {
        if (gamePaused || waypointIndex >= 3)
            return;

        tracerDot.move(unitVector * moveSpeed);
        position = tracerDot.getPosition();
        updateAttachedUI();

        Vector2f diff = waypoints[waypointIndex] - position;
        if (sqrt(diff.x * diff.x + diff.y * diff.y) < moveSpeed * 2.f)
        {
            waypointIndex++;
            computeUnitVector();
        }
    }

    void moveUp() override { moveLinearly(); }
    void moveDown() override { moveLinearly(); }
    void moveLeft() override { moveLinearly(); }
    void moveRight() override { moveLinearly(); }

    Vector2f getVelocity() override { return unitVector * moveSpeed; }

    void draw(RenderWindow &window) override
    {
        if (!shouldDraw())
            return;
        auto [row, col] = screenToTile(position.x, position.y);
        if (checkEndTile(row, col))
            return;

        if (clk.getElapsedTime().asMilliseconds() >= 20 && !gamePaused)
        {
            clk.restart();

            currentFrame = (currentFrame + 1) % 15;
            int r = currentFrame/5 , c = currentFrame %5;
            if(r>1 && c>0) currentFrame = r = c = 0;  
            characterBase.setTextureRect(IntRect({c* 80, r*80}, {80, 80}));

            Vector2i currentTile = {row, col};
            if (currentTile != lastTile)
            {
                masterManager.poll(this, currentTile, {position.x, position.y}, getVelocity());
                lastTile = currentTile;
            }

            moveLinearly();
        }

        updateStatus();
        window.draw(characterBase);
        window.draw(healthBarBase);
        window.draw(healthBarStatus);
    }

    void updateTexture(int a) override {}
};

class Wizard : public Enemy
{
    Clock vanishClock;

public:
    Wizard()
    {
        characterBase.setTexture(&wizard_texture[0]);
        characterBase.setSize({68.f, 68.f});
        characterBase.setScale({0.6f, 0.6f});
        characterBase.setOrigin({32.f, 42.f});
        tracerDot.setPosition(position);
        characterBase.setPosition(position);

        health = 300;
        maxHealth = 300;
        moveSpeed = 1.5f;
        goldReward = 2;
        tracerDot.setRadius(2);
    }
    void attack() {}
    void draw(RenderWindow &window)
    {
        if (!shouldDraw())
            return;
        if (clk.getElapsedTime().asMilliseconds() >= 20)
        {
            clk.restart();
            currentFrame = (currentFrame + 1) % 10;
            int r = currentFrame / 5, c = currentFrame % 5;
            if (r == 1 && c > 2)
                r = c = 0;
            characterBase.setTextureRect(IntRect({c * 68, r * 68}, {68 - 20, 68}));
            Vector2f pos_ = characterBase.getPosition();
            auto [row_, col_] = screenToTile(pos_.x, pos_.y - 10);
            Vector2i currentTile_ = {row_, col_};
            if (currentTile_ == pendingTile)
                pendingCount++;
            else
            {
                pendingTile = currentTile_;
                pendingCount = 0;
            }
            if (pendingCount >= 2 && currentTile_ != lastTile)
            {
                if (vanishClock.getElapsedTime().asSeconds() <= 3)
                {
                    masterManager.poll(this, currentTile_, {position.x, position.y - 30.f}, getVelocity());
                }
                lastTile = currentTile_;
            }
            if (checkEndTile(row_, col_))
                return;
            int dir_ = (lastTile.x >= 0) ? grid[lastTile.x][lastTile.y]->getDirection() : grid[row_][col_]->getDirection();
            updateTexture(dir_);
            switch (dir_)
            {
            case 1:
                moveDown();
                break;
            case 2:
                moveRight();
                break;
            case 3:
                moveLeft();
                break;
            case 4:
                moveUp();
                break;
            default:
                break;
            }
        }
        updateStatus();
        if (vanishClock.getElapsedTime().asSeconds() <= 3)
        {
            window.draw(characterBase);
            window.draw(healthBarBase);
            window.draw(healthBarStatus);
        }
        else if (vanishClock.getElapsedTime().asSeconds() >= 5)
        {
            vanishClock.restart();
        }
    }
    void updateTexture(int a) override
    {
        if (a == 1)
            characterBase.setTexture(&wizard_texture[0]);
        else if (a == 2)
            characterBase.setTexture(&wizard_texture[1]);
        else if (a == 3)
            characterBase.setTexture(&wizard_texture[2]);
        else if (a == 4)
            characterBase.setTexture(&wizard_texture[3]);
    }
};

Enemy *enemies[20] = {};
int enemyCount = 0;

void spawnEnemy(Enemy *e)
{
    if (enemyCount < 20)
    {
        enemies[enemyCount++] = e;
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
        if (enemies[i])
            enemies[i]->draw(window);
    return false;
}

int currentWave = 0;
Clock spawnClock;
bool levelComplete = false;
bool waveInProgress = false;

struct WaveConfig
{
    int goblins, tanks, flashDudes, jahaaz, wizard;
};

WaveConfig waves[5];

int toSpawnGoblin = 0, toSpawnTank = 0, toSpawnFlash = 0, toSpawnJahaaz = 0, toSpawnWizard;
int totalInWave = 0;

bool allDead()
{
    if (enemyCount == 0)
        return false;
    for (int i = 0; i < enemyCount; i++)
        if (enemies[i] && enemies[i]->getHealth() > 0)
            return false;
    return true;
}

void startWave(int waveIndex)
{

    string ws = "Wave : " + to_string(waveIndex + 1) + "/5";
    waveInfo.setString(ws);

    clearEnemies();
    toSpawnGoblin = waves[waveIndex].goblins;
    toSpawnTank = waves[waveIndex].tanks;
    toSpawnFlash = waves[waveIndex].flashDudes;
    toSpawnJahaaz = waves[waveIndex].jahaaz;
    toSpawnWizard = waves[waveIndex].wizard;
    totalInWave = toSpawnGoblin + toSpawnTank + toSpawnFlash + toSpawnJahaaz + toSpawnWizard;
    waveInProgress = true;
    waitingForSpawn = false;
    spawnClock.restart();
    towerMenuEvents.pause();
    anchorHover.pause();
    spawnButton.setFillColor(Color(255, 255, 0, 0));
    cerr << "Wave " << waveIndex + 1 << " starting\n";
}

void updateWaves()
{
    if (!waveInProgress)
        return;

    bool doneSpawning = (toSpawnGoblin == 0 && toSpawnTank == 0 && toSpawnFlash == 0 && toSpawnJahaaz == 0 && toSpawnWizard == 0);

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
        else if (toSpawnJahaaz > 0)
        {
            spawnEnemy(new Jahaaz());
            toSpawnJahaaz--;
        }
        else if (toSpawnWizard > 0)
        {
            spawnEnemy(new Wizard());
            toSpawnWizard--;
        }
    }

    int dead = 0;
    for (int i = 0; i < enemyCount; i++)
    {
        if (enemies[i] && enemies[i]->getHealth() <= 0)
        {
            dead++;
        }
    }

    float prog = (totalInWave > 0) ? (float)dead / totalInWave : 0.f;
    spawnButton.setFillColor(Color(255, 255, 255, (unsigned int)(prog * 255)));

    if (doneSpawning && allDead())
    {
        waveInProgress = false;
        waitingForSpawn = true;
        spawnButton.setFillColor(Color::White);
        towerMenuEvents.resume();
        anchorHover.resume();
        if (currentWave >= 5)
        {
            levelComplete = true;
        }
    }
}

void onSpawnButtonClick()
{
    if (!waitingForSpawn || levelComplete || gameOver)
        return;
    currentWave++;
    if (currentWave <= 4)
        startWave(currentWave - 1);
}

extern EventHandler inGameEvents;
extern displayQueue queue;
Texture scrollPaper("./Assets/overlayFinal.png");
Texture overlayButton_texture("./Assets/overlayButton.png");

void initOverlay()
{
    overlayBox.setSize({500.f, 280.f});
    overlayBox.setOrigin({250.f, 140.f});
    overlayBox.setPosition({600.f, 350.f});
    overlayBox.setTexture(&scrollPaper);
    overlayBox.setTextureRect({{0, 0}, {2528, 1696}});
    // overlayBox.setFillColor(Color(66, 49, 49));
    overlayDarkEffect.setFillColor(Color(13, 22, 31, 75));

    overlayTitle.setCharacterSize(42);
    overlayTitle.setFillColor(Color(25, 18, 87));
    overlayTitle.setPosition({430.f, 240.f});

    replayBtn.setSize({150.f, 50.f});
    replayBtn.setFillColor(Color(255, 255, 255));
    replayBtn.setPosition({390.f, 360.f});
    replayBtn.setTexture(&overlayButton_texture);
    replayBtn.setTextureRect({{42, 144}, {219, 117}});

    menuBtn.setSize({150.f, 50.f});
    menuBtn.setPosition({615.f, 360.f});
    menuBtn.setTexture(&overlayButton_texture);
    menuBtn.setTextureRect({{42, 144}, {219, 117}});

    overlayReplay.setCharacterSize(22);
    overlayReplay.setFillColor(Color::Black);
    overlayReplay.setString("Replay");
    overlayReplay.setPosition({430.f, 365.f});

    overlayMenu.setCharacterSize(22);
    overlayMenu.setFillColor(Color::White);
    overlayMenu.setString("Main Menu");
    overlayMenu.setPosition({648.f, 365.f});
}

void showOverlay(bool win)
{
    overlayActive = true;
    overlayIsWin = win;
    overlayTitle.setString(win ? "Level Complete!" : "Game Over");
    overlayTitle.setFillColor(win ? Color(0, 0, 0) : Color(220, 80, 80));
    towerMenuEvents.pause();
    anchorHover.pause();
    inGameEvents.pause();
    overlayPromptEvents.resume();
}

void resetPlots()
{
    masterManager.clearAll();
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            anchorPlot *ptr = dynamic_cast<anchorPlot *>(grid[i][j]);
            if (ptr != nullptr)
            {
                ptr->destroyTower();
            }
        }
    }
    cerr << "DESTRUCTION : resetPlots() executed succesfully...\n";
}
void resetLevel()
{
    bulletDisplayQueue = TypedDisplayQueue<Bullet, Enemy>();
    allTowers = TowerManager<Tower>();
    playerHealth = 5;
    gameOver = false;
    levelComplete = false;
    currentWave = 0;
    overlayActive = false;
    clearEnemies();
    waveInProgress = false;
    waitingForSpawn = true;
    spawnButton.setFillColor(Color::White);
    // Refill heart points before replaying......
    for (int i = 0; i < 5; i++)
        hearts[i].setTextureRect({{21, 26}, {294, 290}});
    waveInfo.setString("Wave : 0/5");

    masterManager.clearAll();
    towerMenuEvents.resume();
    anchorHover.resume();
    inGameEvents.resume();
    resetPlots();
    cerr << "DESTRUCTION : Level Reset Performed Succesfully....\n";
}

bool drawOverlay(RenderWindow &window)
{
    if (!overlayActive)
        return true;
    window.draw(overlayDarkEffect);
    window.draw(overlayBox);
    window.draw(overlayTitle);
    window.draw(replayBtn);
    window.draw(menuBtn);
    window.draw(overlayReplay);
    window.draw(overlayMenu);
    return false;
}

void handleOverlayClick(const Event &ev)
{
    if (!overlayActive)
        return;
    if (const auto *mb = ev.getIf<Event::MouseButtonPressed>())
    {
        Vector2f pos = Vector2f(mb->position);
        if (replayBtn.getGlobalBounds().contains(pos))
        {
            if (gamePaused)
            {
                inGameEvents.resume();
                overlayPromptEvents.pause();
                gamePaused = false;
                overlayReplay.setString("Replay");
                overlayActive = false;
                towerMenuEvents.resume();
                anchorHover.resume();
                stopDrawing = true;
                handler.resume();
                return;
            }
            resetLevel();
            if (levelComplete)
            {
                currentGold = max(currentGold, tempGold);
            }
            saveBankData();
            inGameEvents.resume();
            towerMenuEvents.resume();
            anchorHover.resume();
            inGameEvents.resume();
        }
        if (menuBtn.getGlobalBounds().contains(pos))
        {
            if (gamePaused)
            {
                gamePaused = false;
            }
            if (levelComplete)
            {
                currentGold = currentGold;
            }
            else
            {
                currentGold = tempGold;
            }
            saveBankData();
            resetLevel();
            queue = displayQueue();
            destroyGrid();
            overlayActive = false;
            stopMenu = false;
            extern bool goBackToMenu;
            goBackToMenu = true;
            inGameEvents.pause();
            anchorHover.pause();
            towerMenuEvents.pause();
            overlayPromptEvents.pause();
            queue.pushBack(displayMenu);
            mainMenuClicks.resume();
        }
    }
    if (const auto *mb = ev.getIf<Event::MouseMoved>())
    {
        Vector2f pos = Vector2f(mb->position);
        if (replayBtn.getGlobalBounds().contains(pos))
        {
            replayBtn.setTextureRect({{346, 139}, {222, 127}});
        }
        else
        {
            replayBtn.setTextureRect({{42, 144}, {219, 117}});
        }
        if (menuBtn.getGlobalBounds().contains(pos))
        {
            menuBtn.setTextureRect({{346, 139}, {222, 127}});
        }
        else
        {
            menuBtn.setTextureRect({{42, 144}, {219, 117}});
        }
    }
}

void loadLevel(int level)
{
    if (level == 1)
    {
        // Focus: Introductory speed and health
        waves[0] = {10, 0, 0, 0, 0}; // Goblins only
        waves[1] = {15, 2, 0, 0, 0}; // Intro TankDude
        waves[2] = {10, 0, 5, 0, 0}; // Intro FlashDude sprinters
        waves[3] = {5, 5, 5, 0, 0};  // Heavy mix of ground units
        waves[4] = {10, 2, 0, 1, 0}; // Boss wave: Goblins + Jahaaz
    }
    else if (level == 2)
    {
        waves[0] = {0, 0, 0, 0, 8};    // Pure Wizard wave
        waves[1] = {10, 0, 5, 0, 5};   // FlashDudes + Wizards
        waves[2] = {0, 10, 0, 0, 5};   // TankDudes shielding Wizards
        waves[3] = {0, 0, 10, 2, 0};   // Double Jahaaz + FlashDude swarm
        waves[4] = {10, 5, 10, 1, 10}; // Total chaos
    }
    else if (level == 3)
    {
        waves[0] = {0, 5, 15, 0, 0};    // Very fast start
        waves[1] = {0, 0, 0, 5, 0};     // Air Raid: 5 Jahaaz
        waves[2] = {0, 15, 0, 0, 15};   // Tanky units and vanishing Wizards
        waves[3] = {25, 0, 20, 0, 0};   // The Swarm: Massive numbers
        waves[4] = {15, 10, 10, 3, 10}; // Final Stand
    }
    currentWave = 0;
    levelComplete = false;
    waveInProgress = false;
    waitingForSpawn = true;
    overlayActive = false;
    clearEnemies();
    spawnButton.setFillColor(Color::White);
    towerMenuEvents.resume();
    anchorHover.resume();
    inGameEvents.resume();
    initOverlay();
    waveInfo.setString("Wave : 0/5");
}

bool drawLevel(RenderWindow &window)
{
    if (!overlayActive)
    {
        extern int tempGold;
        if (gameOver)
        {
            showOverlay(false);
        }
        else if (levelComplete)
        {
            showOverlay(true);
        }
    }
    if (!gamePaused)
        updateWaves();
    drawEnemies(window);
    if (overlayActive)
        drawOverlay(window);
    extern bool goBackToMenu;
    return goBackToMenu;
}