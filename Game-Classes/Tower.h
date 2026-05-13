#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include "masterFile.h"
#include "../Utility-Classes/displayQueue.h"
using namespace std;
using namespace sf;

class Bullet;
class Enemy;
class Tower;
class ShotGun;
extern TowerManager<Tower> allTowers;
extern int towerLevels[5];
bool gamePaused = false;

bool drawBullet(RenderWindow &window, Bullet &obj);
extern displayQueue queue;
extern TypedDisplayQueue<Bullet, Enemy> bulletDisplayQueue;

int openAnchorRow = 0, openAnchorCol = 0;
Texture archer("./Assets/towers.png");
Texture bulletSheet("./Assets/Fireball.png");

class Bullet
{
protected:
    Enemy *targetEnemy = nullptr;
    RectangleShape bulletBase;
    Vector2f startPos, destination, distance;
    float speed, distCoveredX = 0, distCoveredY = 0;
    Vector2f moveData;
    bool available = true;
    int frameNumber = 0;
    int frameRowNumber = 0;
    Clock clk;
    string type = "normal";
    float damageDelt = 10.f; // Standard Damage

public:
    friend class TypedDisplayQueue<Bullet, Enemy>;
    friend class ShotGun;
    Bullet()
    {
        speed = 8;
        bulletBase.setSize({60.0f, 60.0f});
        // bulletBase.setFillColor(Color::Black);
        moveData = {0, 0};
        bulletBase.setTexture(&bulletSheet);
        bulletBase.setTextureRect({{0, 0}, {64, 54}});
    }
    void shoot()
    {
        Vector2f dirVec = destination - startPos;
        float angle = atan2(dirVec.y, dirVec.x) * 180 / 3.14159f;
        if (angle < 0)
            angle += 360;

        if (angle >= 337.5 || angle < 22.5)
            frameRowNumber = 4; // Right
        else if (angle >= 22.5 && angle < 67.5)
            frameRowNumber = 5; // Bottom-Right
        else if (angle >= 67.5 && angle < 112.5)
            frameRowNumber = 6; // Bottom
        else if (angle >= 112.5 && angle < 157.5)
            frameRowNumber = 7; // Bottom-Left
        else if (angle >= 157.5 && angle < 202.5)
        {
            frameRowNumber = 0; // Left
        }
        else if (angle >= 202.5 && angle < 247.5)
            frameRowNumber = 1; // Top-Left
        else if (angle >= 247.5 && angle < 292.5)
            frameRowNumber = 2; // Top
        else if (angle >= 292.5 && angle < 337.5)
            frameRowNumber = 3; // Top-Right
        bulletBase.setPosition(startPos);
        bulletBase.setSize({60.f, 60.f});
        bulletBase.setScale({0.5f, 0.5f});
        distance = destination - startPos;

        float length = sqrt(distance.x * distance.x + distance.y * distance.y);
        moveData.x = (distance.x / length) * speed;
        moveData.y = (distance.y / length) * speed;

        distance.x = abs(distance.x);
        distance.y = abs(distance.y);
        bulletDisplayQueue.pushBack(drawBullet, this, this->targetEnemy);
        available = false;
    }
    void move(Vector2f pos)
    {
        bulletBase.move(pos);
        distCoveredX += abs(pos.x);
        distCoveredY += abs(pos.y);
    }
    void remove()
    {
        distCoveredX = 0;
        distCoveredY = 0;
        moveData = {0, 0};
        available = true;
        frameNumber = 0;
    }
    RectangleShape &getBase()
    {
        return bulletBase;
    }
    friend bool drawBullet(RenderWindow &window, Bullet &obj);
    friend class Tower;
};

bool drawBullet(RenderWindow &window, Bullet &obj)
{
    if (gamePaused)
        ;
    if (obj.clk.getElapsedTime().asMilliseconds() >= 15 && !gamePaused)
    {
        obj.frameNumber += 1;
        obj.bulletBase.setTextureRect({{(obj.frameNumber % 8) * 64, obj.frameRowNumber * 54}, {64, 54}});
        obj.clk.restart();
        obj.move(obj.moveData);
        if (obj.distCoveredX >= obj.distance.x && obj.distCoveredY >= obj.distance.y)
        {
            obj.remove();
            return true;
        }
    }
    if (obj.type != "slowEffect")
    {
        window.draw(obj.bulletBase);
    }
    return false;
}

class Tower : public Entity
{
protected:
    string type;
    RectangleShape towerBase;
    Texture *tex;
    Vector2f position;
    int ammunition, currentAmmo;
    int plotRow, plotCol;
    float attackRadius;
    Bullet magazine[15];
    Clock attackTimer;
    float hitDelay, bulletSpeed = 6.0f;
    string shootType = "normal";
    float damageInduced = 10.f;
    int eventManagerIndex;

public:
    Tower(int a, int b)
    {
        eventManagerIndex = allTowers.pushBack(this);
        towerBase.setSize({200.f, 400.f});
        towerBase.setOrigin({0.f, 400.0f});
        type = "undef";
        plotRow = a;
        plotCol = b;
        ammunition = 0;
        hitDelay = 1000;
        void (*onClick)(Event &ev);
    }
    virtual void draw(RenderWindow &window)
    {
        window.draw(towerBase);
    }
    virtual void attack(Enemy *target, Vector2f targetPos, Vector2f targetVelocity)
    {
        if (gamePaused)
            return;
        if (attackTimer.getElapsedTime().asMilliseconds() <= hitDelay)
        {
            return;
        }
        attackTimer.restart();

        for (int i = 0; i < 15; i++)
        {
            if (magazine[i].available)
            {
                magazine[i].targetEnemy = target;
                float bulletSpeed = magazine[i].speed;

                Vector2f relativePos = targetPos - this->position;
                float distance = sqrt(relativePos.x * relativePos.x + relativePos.y * relativePos.y);
                float timeToImpact = distance / bulletSpeed;
                Vector2f predictedPos = targetPos + (targetVelocity * timeToImpact);

                magazine[i].startPos = {this->position.x, this->position.y - 68};
                magazine[i].destination = predictedPos;
                magazine[i].speed = bulletSpeed;
                magazine[i].type = shootType;
                magazine[i].damageDelt = damageInduced;
                magazine[i].shoot();
                break;
            }
        }
    }
    virtual void setPosition(Vector2f pos)
    {
        towerBase.setPosition(pos);
        position = pos;
    }
    virtual void scanTiles()
    {
        for (int i = 0; i < 4; i++)
        {
        }
    }
    virtual int getRadius()
    {
        return attackRadius;
    }
    virtual void handleEvents(Event &ev)
    {
    }
    virtual ~Tower()
    {
        tex = nullptr;
    }
};

class SniperTower : public Tower
{
public:
    SniperTower(int a, int b) : Tower(a, b)
    {
        int level = towerLevels[0];
        damageInduced = 40.f + (level-1)*10.f;
        attackRadius = 20;
        hitDelay = 750;
        bulletSpeed = 15.0f;
        towerBase.setSize({123.f, 159.f});
        towerBase.setOrigin({61.f, 158.f});
        towerBase.setTexture(&archer);
        towerBase.setTextureRect({{14, 14}, {126, 151}});
        towerBase.setScale({0.4f, 0.4f});
    }
    int getRadius() override
    {
        switch (towerLevels[0])
        {
        case 1:
            return 15;
            break;
        case 2:
            return 18;
            break;
        case 3:
            return 20;
        case 4:
            return 23;
        case 5:
            return 25;
        default:
            return 20;
        }
    }
};

class MachineGunTower : public Tower
{
public:
    MachineGunTower(int a, int b) : Tower(a, b)
    {
        int level = towerLevels[1];
        damageInduced = 20.f + (level-1)*6.f;
        attackRadius = 9;
        hitDelay = 15;
        towerBase.setSize({123.f, 159.f});
        towerBase.setOrigin({61.f, 158.f});
        towerBase.setTexture(&archer);
        towerBase.setTextureRect({{17, 191}, {123, 159}});
        towerBase.setScale({0.4f, 0.4f});
    }
    int getRadius() override
    {
        switch (towerLevels[1])
        {
        case 1:
            return 9;
            break;
        case 2:
            return 12;
            break;
        case 3:
            return 14;
        case 4:
            return 16;
        case 5:
            return 18;
        default:
            return 20;
        }
    }
};

class CannonTower : public Tower
{
public:
    CannonTower(int a, int b) : Tower(a, b)
    {
        int level = towerLevels[2];
        damageInduced = 75.f + (level-1)*20.f;
        attackRadius = 20;
        hitDelay = 2500;
        towerBase.setSize({112.f, 137.f});
        towerBase.setOrigin({56.f, 137.f});
        towerBase.setTexture(&archer);
        towerBase.setTextureRect({{148, 13}, {127, 156}});
        towerBase.setScale({0.4f, 0.4f});
    }
    int getRadius() override
    {
        switch (towerLevels[2])
        {
        case 1:
            return 15;
            break;
        case 2:
            return 20;
            break;
        case 3:
            return 22;
        case 4:
            return 24;
        case 5:
            return 26;
        default:
            return 28;
        }
    }
};

class SlowTower : public Tower
{
public:
    SlowTower(int a, int b) : Tower(a, b)
    {
        int level = towerLevels[3];
        damageInduced = 3.f + (level-1)*10.f;
        attackRadius = 15;
        hitDelay = 0;
        bulletSpeed = 20.0f;
        shootType = "slowEffect";
        towerBase.setSize({123.f, 159.f});
        towerBase.setOrigin({61.f, 158.f});
        towerBase.setTexture(&archer);
        towerBase.setTextureRect({{280, 10}, {122, 156}});
        towerBase.setScale({0.4f, 0.4f});
    }
    int getRadius() override
    {
        switch (towerLevels[3])
        {
        case 1:
            return 15;
            break;
        case 2:
            return 18;
            break;
        case 3:
            return 20;
        case 4:
            return 22;
        case 5:
            return 24;
        default:
            return 20;
        }
    }
};

class ShotGun : public Tower
{
public:
    ShotGun(int a, int b) : Tower(a, b)
    {
        int level = towerLevels[0];
        damageInduced = 15.f + (level-1)*15.f;
        attackRadius = 12;
        hitDelay = 700;
        bulletSpeed = 7.0f;
        shootType = "spread";
        towerBase.setSize({123.f, 159.f});
        towerBase.setOrigin({61.f, 158.f});
        towerBase.setTexture(&archer);
        towerBase.setTextureRect({{411, 184}, {122, 156}});
        towerBase.setScale({0.4f, 0.4f});
    }

    void attack(Enemy *target, Vector2f targetPos, Vector2f targetVelocity) override
    {
        if (attackTimer.getElapsedTime().asMilliseconds() <= hitDelay)
            return;
        attackTimer.restart();
        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < 15; i++)
            {
                if (magazine[i].available)
                {
                    magazine[i].targetEnemy = target;

                    Vector2f spreadPos = targetPos;
                    spreadPos.x += (j - 1) * 30.f; // sequence : -1, 0,1
                    spreadPos.y += (j - 1) * 20.f;

                    magazine[i].startPos = {this->position.x, this->position.y - 68};
                    magazine[i].destination = spreadPos;
                    magazine[i].speed = bulletSpeed;
                    magazine[i].type = shootType;
                    magazine[i].damageDelt = damageInduced;
                    magazine[i].shoot();
                    break;
                }
            }
        }
    }

    int getRadius() override
    {
        switch (towerLevels[4])
        {
        case 1:
            return 12;
            break;
        case 2:
            return 14;
            break;
        case 3:
            return 16;
            break;
        case 4:
            return 18;
            break;
        case 5:
            return 20;
            break;
        default:
            return 12;
            break;
        }
    }
};

// AnchroPlot holds Tower*
// Menu Chooses a Tower -> Tower* becomes the selected tower
// Sets Tower Position to plot Position & Assigns it the tile coords.
//
// AnchorPlot also makes the tower draw calls.
