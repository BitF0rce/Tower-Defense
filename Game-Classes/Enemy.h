#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Plot.h"
#include "masterFile.h"
using namespace std;
using namespace sf;


class Enemy : public Entity
{
protected:
    Vector2f position;
    int strengthLevel;
    float attackDelay; // ms
    std::optional<Sprite> characterBase;
    int currentFrame;
    float moveSpeed = 1.0f;
public:
    Clock clk;
    Enemy()
    {
        position = {0.0f, 0.0f};
        strengthLevel = 1;
        attackDelay = 1400;
    }

    virtual void moveDown() =0;
    virtual void moveRight() =0;
    virtual void moveUp() =0;
    virtual void moveLeft() =0;
    virtual void attack() =0;
};

Texture emptyTexture("/home/shaaf/Desktop/Tower-Defense/Assets/enemeies/Monster_10/PNG/PNG/Idle/7.png");
Texture idle[18];

class Goblin : public Enemy
{
public:
    Goblin()
    {
        idle[0] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_000.png");
        idle[1] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_001.png");
        idle[2] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_002.png");
        idle[3] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_003.png");
        idle[4] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_004.png");
        idle[5] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_005.png");
        idle[6] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_006.png");
        idle[7] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_007.png");
        idle[8] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_008.png");
        idle[9] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_009.png");
        idle[10] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_010.png");
        idle[11] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_011.png");
        idle[12] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_012.png");
        idle[13] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_013.png");
        idle[14] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_014.png");
        idle[15] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_015.png");
        idle[16] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_016.png");
        idle[17] = Texture("./Assets/enemeies/Monster_10/PNG/PNG/Walking/0_Monster_Walking_017.png");
        position = {215.0f, 34.0f};
        characterBase.emplace(idle[0]);
        characterBase->setPosition(position);
    }
    void moveDown()
    {
        characterBase->move({moveSpeed , moveSpeed/2.0f});
    }
    void moveRight(){
        characterBase->move({moveSpeed , -moveSpeed/2.0f});
    }
    void moveLeft(){
        
    }
    void moveUp(){

    }
    void attack(){

    }
    void draw(RenderWindow &window)
    {
        if (clk.getElapsedTime().asMilliseconds() >= 10)
        {
            clk.restart();
            currentFrame = (currentFrame + 1) % 17;
            characterBase->setTexture(idle[currentFrame]);
            characterBase->setScale({0.2f, 0.2f});
            characterBase->setTextureRect({{140,60},{220,220}});
            Vector2f pos = characterBase->getPosition();
            int dir = getDirection(pos.x , pos.y);
            // cerr<<"Move..dir = "<<dir<<endl;
            switch (dir)
            {
            case 0:
                break;
            case 1:
                // moveDown();
                break;
            case 2:
                moveRight();
                break;
            default:
                break;
            }
        }
        window.draw(*characterBase);
    }
};

Goblin g;

bool drawEnemy(RenderWindow &window)
{
    g.draw(window);
    return false;
}
