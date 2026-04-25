#include<iostream>
#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<math.h>
#include<SFML/Window.hpp>

using namespace std;
using namespace sf;

class RoundedRectangle{
    float height , width, radius;
    Vector2f position;
    ConvexShape base;
    public:
    RoundedRectangle(){
        height=0;
        width=0;
        radius= 0;
    }
    RoundedRectangle(float w , float h , float radius): height(h) , width(w){
        base.setPointCount(360);
        this->radius= radius;
        float theta = M_PI , scaler = (M_PI/2.0f) / 90.0f;
        position = {radius* cos(theta) , radius* sin(theta)};
        int i=0;
        for(i ; i<90 ; i++){
            base.setPoint(i , {radius* cos(theta) , radius* sin(theta)});
            theta += scaler;
        }
        for(i ; i<180 ; i++){
            base.setPoint(i , {radius* cos(theta) + width , radius* sin(theta)});
            theta += scaler;
        }
        for(i ; i<270 ; i++){
            base.setPoint(i , {radius* cos(theta)+width , radius* sin(theta)+height});
            theta += scaler;
        }
        for(i ; i<360 ; i++){
            base.setPoint(i , {radius* cos(theta) , radius* sin(theta) + height});
            theta+= scaler;
        }
    }
    RoundedRectangle(RoundedRectangle& src): height(src.height) , width(src.width){
        base.setPointCount(360);
        this->radius= src.radius;
        float theta = M_PI , scaler = (M_PI/2.0f) / 90.0f;
        position = {radius* cos(theta) , radius* sin(theta)};
        int i=0;
        for(i ; i<90 ; i++){
            base.setPoint(i , {radius* cos(theta) , radius* sin(theta)});
            theta += scaler;
        }
        for(i ; i<180 ; i++){
            base.setPoint(i , {radius* cos(theta) + width , radius* sin(theta)});
            theta += scaler;
        }
        for(i ; i<270 ; i++){
            base.setPoint(i , {radius* cos(theta)+width , radius* sin(theta)+height});
            theta += scaler;
        }
        for(i ; i<360 ; i++){
            base.setPoint(i , {radius* cos(theta) , radius* sin(theta) + height});
            theta+= scaler;
        }
    }

    void draw(RenderWindow& window){
        window.draw(base);
    }
    void setFillColor(Color c){
        base.setFillColor(c);
    }
    void setPosition(Vector2f p){
        position = p;
        base.setPosition(position);
    }
    void move(Vector2f mv){
        base.move(mv);
    }
    void setWidth(float wid){
        float scalerFactor = wid - width;
        for(int i=90 ; i<270 ; i++){
            base.setPoint(i , {base.getPoint(i).x+ scalerFactor, base.getPoint(i).y});
        }
        width = wid; 
    }
    void setHeight(float he){
        float scalerFactor = height - he;
        for(int i=180 ; i<360 ; i++){
            base.setPoint(i , {base.getPoint(i).x, base.getPoint(i).y+ scalerFactor});
        }
        height = he; 
    }
    void setSize(Vector2f s){
        if(s.x != width){
            setWidth(s.x);
        }
        if(s.y != height){
            setHeight(s.y);
        }
    }
    Vector2f getSize(){
        return {width , height};
    }
    Vector2f getPosition(){
        return position;
    }
};