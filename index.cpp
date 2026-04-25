#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include "./Screens/loading.h"
using namespace std;
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

int main()
{
    ContextSettings settings;
    settings.antiAliasingLevel = 8;
    displayQueue queue;
    RenderWindow window(VideoMode({1100, 600}), "Tower Defense", Style::Default, State::Windowed, settings);
    queue.pushBack(drawLoadingScreen);
    loadScreen(window);
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }
        }
        window.clear(Color(29, 29, 29));
        queue.runAll(window);
        window.display();
    }
}