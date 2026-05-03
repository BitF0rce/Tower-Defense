#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include<string>
#include"masterFile.h"
using namespace std;
using namespace sf;

Texture archer("/home/shaaf/Desktop/Tower-Defense/Assets/towers.png");
class Tower: public Entity{
    protected:
    string type;
    RectangleShape towerBase;
    Texture* tex;
    Vector2f position;
    int ammunition;
    int plotRow,plotCol;
    float attackRadius;
    public:
    Tower(int a , int b){
        towerBase.setSize({200.f , 400.f});
        towerBase.setOrigin({0.f,400.0f});
        type = "undef";
        plotRow= a;
        plotCol = b;
        ammunition =0;
    }
    virtual void draw(RenderWindow& window){
        window.draw(towerBase);
    }
    virtual void attack()=0;
    virtual void setPosition(Vector2f pos){
        towerBase.setPosition(pos);
    }
};

class ArcherTower:public Tower{
    public:
    ArcherTower(int a, int b): Tower(a,b){
        attackRadius = 50;
        towerBase.setSize({112.f , 137.f});
        towerBase.setOrigin({56.f, 137.f});
        towerBase.setTexture(&archer);
        towerBase.setTextureRect({{14,14},{126,151}});
        towerBase.setScale({0.4f, 0.4f});
    }
    void attack(){

    }
};

// AnchroPlot holds Tower*
// Menu Chooses a Tower -> Tower* becomes the selected tower
// Sets Tower Position to plot Position & Assigns it the tile coords.
// 
// AnchorPlot also makes the tower draw calls. 

