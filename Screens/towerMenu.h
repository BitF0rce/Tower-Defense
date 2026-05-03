#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include"../Utility-Classes/displayQueue.h"
using namespace sf;
using namespace std;

bool stopDrawing = true;
float r1 = 70, r2 = 110;
CircleShape inner(r1);
CircleShape outer(r2);
RectangleShape tower;
Texture towerText("/home/shaaf/Desktop/Tower-Defense/Assets/arrow.png");

void initiateTowerMenu(Vector2f pos){
    inner.setFillColor(Color(0,0,0,170));
    outer.setFillColor(Color(0,20,25,100));
    inner.setPosition(pos);
    outer.setPosition(pos);
    inner.setOrigin({r1 , r1});
    outer.setOrigin({r2 , r2});

        tower.setSize({70.f , 70.f});
        // towers[i].setTextureRect({{30 , 30} , {140 , 150}});
        tower.setScale({0.6f,0.6f});
        tower.setTexture(&towerText);

    tower.setPosition({inner.getPosition().x , inner.getPosition().y-r2});
}

bool drawTowerMenu(RenderWindow& window){
    window.draw(outer);
    window.draw(inner);
    window.draw(tower);
    return stopDrawing;
}

// Is the tower spot occupied.
// Show the respectively selected tower.
// Make Tower Class along with it's Attack Radius detection and Attack mechanism
// Add Health Dynamics to the Enemy Class which are affected by attack.
// Improve Waypoints
// An Enemy must always be animated-> Make Animation Queue which holds All Enemies' Animation states. 
// 