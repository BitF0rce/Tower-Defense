#pragma once
#include<SFML/Graphics.hpp>
using namespace sf;
typedef bool (*drawerFunction)(RenderWindow &window);

class displayQueue
{
    drawerFunction *queue;
    int length = 0;
    static bool emptyFunction(RenderWindow &window) {return true;};

public:
    displayQueue()
    {
        queue = nullptr;
    }
    void pushBack(drawerFunction func)
    {
        drawerFunction *temp = new drawerFunction[length + 1];
        if (queue != nullptr)
        {
            for (int i = 0; i < length; i++)
            {
                temp[i] = queue[i];
            }
        }
        temp[length] = func;
        delete[] queue;
        queue = temp;
        length++;
    }
    void pop()
    {
        if (length <= 0)
        {
            length = 0;
            return;
        }
        else if (length == 1)
        {
            delete[] queue;
            queue = nullptr;
            length = 0;
            return;
        }
        drawerFunction *temp = new drawerFunction[length - 1];
        for (int i = 0; i < length - 1; i++)
        {
            temp[i] = queue[i];
        }
        delete[] queue;
        queue = temp;
        length--;
    }
    void remove(int index)
    {
        if (index < 0 || index >= length)
            return;

        if (length == 1)
        {
            delete[] queue;
            queue = nullptr;
            length = 0;
            return;
        }

        drawerFunction *temp = new drawerFunction[length - 1];
        int tempIdx = 0;
        for (int i = 0; i < length; i++)
        {
            if (i == index)
                continue; // Skip the one we are removing
            temp[tempIdx] = queue[i];
            tempIdx++;
        }
        delete[] queue;
        queue = temp;
        length--;
    }
    void runAll(RenderWindow &window)
    {
        for (int i = 0; i < length; i++)
        {
            if (queue[i](window))
            {
                queue[i] = emptyFunction;
            }
        }
        for (int i = 0; i < length; i++)
        {
            if (queue[i] == emptyFunction)
            {
                remove(i);
                i--;
            }
        }
    }
    ~displayQueue()
    {
        delete[] queue;
        length = 0; // LoL
    }
};

typedef void (*menuEvent)(const Event &ev);
class EventHandler{
    menuEvent* funcArray = nullptr;
    int count=0;
    bool isPaused = true;
    public:
    EventHandler(){}
    void addEvent(menuEvent func){
        menuEvent* temp = new menuEvent[count+1];
        for(int i=0;i<count;i++)
        temp[i] = funcArray[i];
        temp[count] = func;
        if(funcArray!= nullptr)
        delete[] funcArray;
        funcArray = temp;
        count++;
    }
    void pause(){
        isPaused = true;
    }
    void resume(){
        isPaused = false;
    }
    void handleEvents(const Event &ev){
        if(isPaused){
            return;
        }
        for(int i=0; i<count;i++){
            funcArray[i](ev);
        }
    }
    ~EventHandler(){
        delete[] funcArray;
    }
};
