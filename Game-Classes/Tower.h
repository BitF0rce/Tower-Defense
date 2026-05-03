#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include<string>
#include"masterFile.h"
using namespace std;
using namespace sf;

int openAnchorRow =0 , openAnchorCol = 0; 
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
    virtual void attack(Vector2f pos, float speed)=0;
    virtual void setPosition(Vector2f pos){
        towerBase.setPosition(pos);
    }
    virtual void scanTiles(){
        for(int i=0;i<4;i++){
        }
    }
    virtual int getRadius(){
        cerr<<"Inside Tower...";
        return attackRadius;
    }
};

class ArcherTower:public Tower{
    public:
    ArcherTower(int a, int b): Tower(a,b){
        attackRadius = 4;
        towerBase.setSize({112.f , 137.f});
        towerBase.setOrigin({56.f, 137.f});
        towerBase.setTexture(&archer);
        towerBase.setTextureRect({{14,14},{126,151}});
        towerBase.setScale({0.4f, 0.4f});
        // towerBase.setOutlineColor(Color::Black);
        // towerBase.setOutlineThickness(5);
    }
    void attack(Vector2f pos, float speed){
    }
};


class IceTower:public Tower{
    public:
    IceTower(int a, int b): Tower(a,b){
        attackRadius = 4;
        towerBase.setSize({122.f , 158.f});
        towerBase.setOrigin({61.f, 158.f});
        towerBase.setTexture(&archer);
        towerBase.setTextureRect({{412,191},{113,155}});
        towerBase.setScale({0.4f, 0.4f});
    }
    void attack(Vector2f pos, float speed){
        cerr<<"Enemy ja raha hai , Pakro !!!!\n";
    }
};


// AnchroPlot holds Tower*
// Menu Chooses a Tower -> Tower* becomes the selected tower
// Sets Tower Position to plot Position & Assigns it the tile coords.
// 
// AnchorPlot also makes the tower draw calls. 

