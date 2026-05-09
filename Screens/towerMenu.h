#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include"../Utility-Classes/displayQueue.h"
using namespace sf;
using namespace std;

bool stopDrawing = true;
float r1 = 70, r2 = 120;
CircleShape inner(r1);
CircleShape outer(r2);
RectangleShape tower[5];
Texture towerText("/home/shaaf/Desktop/Tower-Defense/Assets/TowerMenuIcons.png");

void initiateTowerMenu(Vector2f pos){
    inner.setFillColor(Color(0,0,0,170));
    outer.setFillColor(Color(0,20,25,100));
    inner.setPosition(pos);
    outer.setPosition(pos);
    inner.setOrigin({r1, r1});
    outer.setOrigin({r2, r2});

    float angleStep = 360.f / 5.f;
    float PI = 3.14159f;
    float radialDistance = (r1 + r2) / 2.f;

    for(int i = 0; i < 5; i++){
        tower[i].setTextureRect({{17+(i*133),109},{133,135}});
        tower[i].setSize({70.f, 70.f});
        tower[i].setOrigin({35.f, 35.f});
        tower[i].setScale({0.6f, 0.6f});
        tower[i].setTexture(&towerText);
        
        float angle = (i * angleStep - 90.f) * (PI / 180.f);
        float itemX = pos.x + radialDistance * cos(angle);
        float itemY = pos.y + radialDistance * sin(angle);

        tower[i].setPosition({itemX, itemY});
        // tower[i].setOutlineColor(Color::Green);
        // tower[i].setOutlineThickness(3);
    }
}
bool drawTowerMenu(RenderWindow& window){
    window.draw(outer);
    window.draw(inner);
    for(int i=0;i<5;i++)
    window.draw(tower[i]);
    return stopDrawing;
}