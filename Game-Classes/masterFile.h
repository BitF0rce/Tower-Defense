#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
using namespace std;
using namespace sf;

class Entity{
protected:
    float maxHealth;
    float health;
public:
    virtual void draw(RenderWindow& window) = 0;
};

