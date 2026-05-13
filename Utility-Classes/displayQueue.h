#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;
typedef bool (*drawerFunction)(RenderWindow &window);

class displayQueue
{
    drawerFunction *queue;
    int length = 0;
    static bool emptyFunction(RenderWindow &window) { return true; };

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
class EventHandler
{
    menuEvent *funcArray = nullptr;
    int count = 0;
    bool isPaused = true;

public:
    EventHandler() {}
    void addEvent(menuEvent func)
    {
        menuEvent *temp = new menuEvent[count + 1];
        for (int i = 0; i < count; i++)
            temp[i] = funcArray[i];
        temp[count] = func;
        if (funcArray != nullptr)
            delete[] funcArray;
        funcArray = temp;
        count++;
    }
    void pause()
    {
        isPaused = true;
    }
    void resume()
    {
        isPaused = false;
    }
    void handleEvents(const Event &ev)
    {
        if (isPaused)
        {
            return;
        }
        for (int i = 0; i < count; i++)
        {
            funcArray[i](ev);
        }
    }
    ~EventHandler()
    {
        delete[] funcArray;
    }
};

template <typename T, typename K>
class TypedDisplayQueue
{
    typedef bool (*drawerFunction)(RenderWindow &window, T &obj);

    struct Entry
    {
        drawerFunction func;
        T *obj;
        K *target;
        bool done = false;
    };

    Entry *queue = nullptr;
    int length = 0;

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
        Entry *temp = new Entry[length - 1];
        int tempIdx = 0;
        for (int i = 0; i < length; i++)
        {
            if (i == index)
                continue;
            temp[tempIdx++] = queue[i];
        }
        delete[] queue;
        queue = temp;
        length--;
    }

public:
    void pushBack(drawerFunction func, T *obj, K *target)
    {
        Entry *temp = new Entry[length + 1];
        for (int i = 0; i < length; i++)
            temp[i] = queue[i];
        temp[length] = {func, obj, target};
        delete[] queue;
        queue = temp;
        length++;
    }

    void runAll(RenderWindow &window)
    {
        for (int i = 0; i < length; i++)
        {
            if (queue[i].func(window, *queue[i].obj))
                queue[i].done = true;
        }
        for (int i = 0; i < length; i++)
        {
            if (queue[i].done)
            {
                if (queue[i].obj->bulletBase.getGlobalBounds().findIntersection(queue[i].target->characterBase.getGlobalBounds()))
                {
                    if (queue[i].obj->type == "slowEffect")
                    {
                        queue[i].target->applyFreeze();
                    }
                    queue[i].target->collide(queue[i].obj->damageDelt);
                }
                queue[i].obj->bulletBase.setSize({0, 0});
                queue[i].obj->bulletBase.setPosition({0, 0});
                remove(i);
                i--;
            }
        }
    }

    ~TypedDisplayQueue()
    {
        delete[] queue;
    }
};

template <typename Tow>
class TowerManager
{
    Tow **stack = nullptr;
    int count = 0;

public:
    int pushBack(Tow *input)
    {
        Tow **temp = new Tow *[count + 1];
        for (int i = 0; i < count; i++)
        {
            temp[i] = stack[i];
        }
        temp[count] = input;
        delete[] stack;
        stack = temp;
        count++;
        return count - 1;
    }

    void removeAtIndex(int index)
    {
        if (index < 0 || index >= count)
            return;
        if (count == 1)
        {
            delete[] stack;
            stack = nullptr;
            count = 0;
            return;
        }

        Tow **temp = new Tow *[count - 1];
        int tempIdx = 0;
        for (int i = 0; i < count; i++)
        {
            if (i != index)
            {
                temp[tempIdx++] = stack[i];
            }
        }
        delete[] stack;
        stack = temp;
        count--;
    }

    void handleEvents(Event &ev)
    {
        for (int i = 0; i < count; i++)
        {
            if (stack[i] == nullptr)
                continue;
            stack[i]->handleEvents(ev);
        }
    }

    ~TowerManager()
    {
        delete[] stack;
    }
};
struct CircularButton
{
    CircleShape base;
    CircleShape visual;
    Vector2f position;
    bool hovering;
    Clock moveClock;
    float rad;
};

void checkMove(RectangleShape &testBox, const Event &ev)
{
    if (const auto click = ev.getIf<Event::MouseButtonPressed>())
    {
        Vector2f pos = {
            (float)click->position.x,
            (float)click->position.y};

        testBox.setPosition(pos);
    }

    else if (const auto key = ev.getIf<Event::KeyPressed>())
    {
        if (key->code == Keyboard::Key::Left)
        {
            testBox.setSize({testBox.getSize().x - 1.f,
                             testBox.getSize().y});
        }
        else if (key->code == Keyboard::Key::Right)
        {
            testBox.setSize({testBox.getSize().x + 1.f,
                             testBox.getSize().y});
        }
        else if (key->code == Keyboard::Key::Up)
        {
            testBox.setSize({testBox.getSize().x,
                             testBox.getSize().y - 1.f});
        }
        else if (key->code == Keyboard::Key::Down)
        {
            testBox.setSize({testBox.getSize().x,
                             testBox.getSize().y + 1.f});
        }
        else if (key->code == Keyboard::Key::O)
        {
            Vector2f pos = testBox.getPosition();
            Vector2f size = testBox.getSize();

            std::cout << "Position: ("
                      << pos.x << ", " << pos.y << ")\n";

            std::cout << "Size: ("
                      << size.x << ", " << size.y << ")\n";
        }
    }
}