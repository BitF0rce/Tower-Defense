#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cmath>
using namespace std;
using namespace sf;

int main()
{
    ContextSettings settings;
    settings.antiAliasingLevel = 8;
    RenderWindow window(VideoMode({1200, 700}), "Tower Defense", Style::Default, State::Windowed, settings);

    Texture Path("./Assets/isometric/separated-images/tile_009.png");
    Texture Path2("./Assets/isometric/separated-images/tile_061.png");
    cout << Path.getSize().x << " , " << Path.getSize().y;
    float tileW = 64, tileH = 32, originX = 500.0f, originY = 50.0f;
    ConvexShape grid[10][10];
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            grid[i][j].setPointCount(6); 
            grid[i][j].setPoint(0, {tileW / 2.f, 0.f});
            grid[i][j].setPoint(1, {tileW, tileH});
            grid[i][j].setPoint(2, {tileW, tileH + (0.4f * tileH)});
            grid[i][j].setPoint(3, {tileW / 2.f, tileH * 2});
            grid[i][j].setPoint(4, {0.f, tileH + (0.4f * tileH)});
            grid[i][j].setPoint(5, {0.f, tileH});

            float screenX = originX + (j - i) * (tileW / 2);
            float screenY = originY + (j + i) * (tileH / 2);
            grid[i][j].setPosition({screenX, screenY});
            if (i == 0 || j == 0 || i == 9 || j == 9)
            {
                grid[i][j].setTexture(&Path2);
            }
            else
            {
                grid[i][j].setTexture(&Path);
            }
            grid[i][j].setTextureRect(IntRect({0, 0}, {32, 32}));
        }
    }
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }
            if (const auto *mouse = event->getIf<sf::Event::MouseMoved>())
            {
                float screenX = mouse->position.x;
                float screenY = mouse->position.y;

                float dx = screenX - originX;
                float dy = screenY - originY;

                float col = (dx / (tileW / 2.f) + dy / (tileH / 2.f)) / 2.f;
                float row = (dy / (tileH / 2.f) - dx / (tileW / 2.f)) / 2.f;

                int tileCol = (int)floor(col)-1;
                int tileRow = (int)floor(row);

                cout << "Clicked: " << tileRow << " , " << tileCol << std::endl;
                if (tileRow >= 0 && tileRow < 10 && tileCol >= 0 && tileCol < 10)
                {
                    cout << "Hovered: " << tileRow << " , " << tileCol << endl;
                    grid[tileRow][tileCol].move({0, 2});
                }
            }
        }
        window.clear(Color(29, 29, 29));
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                window.draw(grid[i][j]);
            }
        }
        window.display();
    }
}