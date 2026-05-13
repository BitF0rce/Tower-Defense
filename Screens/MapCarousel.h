#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <algorithm>

using namespace sf;
using namespace std;

Clock awaitTransit;

Texture dummyTexture("./Assets/towers.png");

struct MapCard
{
    RectangleShape sprite;
    int levelID = -1;
};

class MapCarousel
{
private:
    MapCard cards[3];
    int cardCount = 0;

    RectangleShape frame;
    RectangleShape btnLeft, btnRight;

    int currentIndex = 0;
    float cardWidth = 300.f;
    float cardHeight = 300.f;
    Vector2f centerPos = {600.f, 350.f};

    float slideOffset = 0.f;
    bool isSliding = false;
    int targetIndex = 0;

public:
    MapCarousel()
    {
        frame.setSize({cardWidth + 10.f, cardHeight + 10.f});
        // frame.setOutlineThickness(5.f);
        // frame.setOutlineColor(Color::Yellow);
        frame.setFillColor(Color::Transparent);
        frame.setOrigin({frame.getSize().x / 2.f, frame.getSize().y / 2.f});
        frame.setPosition(centerPos);

        btnLeft.setSize({60.f, 60.f});
        btnRight.setSize({60.f, 60.f});
        btnLeft.setOrigin({30.f, 30.f});
        btnRight.setOrigin({30.f, 30.f});
        btnLeft.setPosition({centerPos.x - 280.f, centerPos.y});
        btnRight.setPosition({centerPos.x + 280.f, centerPos.y});

        btnLeft.setFillColor(Color(100, 100, 100));
        btnRight.setFillColor(Color(100, 100, 100));
    }

    void addMap(const Texture &tex, int id)
    {
        if (cardCount < 3)
        {
            cards[cardCount].sprite.setTexture(&tex);
            Vector2f texSize = Vector2f(tex.getSize());
            cards[cardCount].sprite.setSize(texSize);

            FloatRect bounds = cards[cardCount].sprite.getLocalBounds();
            cards[cardCount].sprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});

            cards[cardCount].levelID = id;
            cardCount++;
        }
    }

    void update()
    {
        if (isSliding)
        {
            float destOffset = (targetIndex * -450.f);
            float speed = 20.f;

            if (abs(slideOffset - destOffset) < speed)
            {
                slideOffset = destOffset;
                currentIndex = targetIndex;
                isSliding = false;
            }
            else
            {
                slideOffset += (targetIndex > currentIndex) ? -speed : speed;
            }
        }
    }

    void draw(RenderWindow &window)
    {
        window.draw(frame);
        window.draw(btnLeft);
        window.draw(btnRight);

        for (int i = 0; i < cardCount; i++)
        {
            float xPos = centerPos.x + (i * 450.f) + slideOffset;

            float dist = abs(xPos - centerPos.x);
            float t = min(dist / 450.f, 1.0f);

            float dynamicScaleFactor = 1.0f - (t * 0.25f);

            unsigned int brightness = static_cast<unsigned int>(255 - (t * 155));

            Vector2f baseSize = cards[i].sprite.getSize();
            cards[i].sprite.setScale({(cardWidth * dynamicScaleFactor) / baseSize.x,
                                      (cardHeight * dynamicScaleFactor) / baseSize.y});

            Color dynamicColor(brightness, brightness, brightness);
            cards[i].sprite.setFillColor(dynamicColor);
            cards[i].sprite.setOutlineColor(dynamicColor);
            cards[i].sprite.setPosition({xPos, centerPos.y});

            if (xPos > -200.f && xPos < 1400.f)
            {
                window.draw(cards[i].sprite);
            }
        }
    }

    void handleInput(Vector2f mousePos)
    {
        if (isSliding)
            return;

        if (btnLeft.getGlobalBounds().contains(mousePos) && currentIndex > 0)
        {
            targetIndex = currentIndex - 1;
            isSliding = true;
        }
        if (btnRight.getGlobalBounds().contains(mousePos) && currentIndex < cardCount - 1)
        {
            targetIndex = currentIndex + 1;
            isSliding = true;
        }
    }
    void handleHover(Vector2f mousePos)
    {
        if (btnLeft.getGlobalBounds().contains(mousePos))
        {
            btnLeft.setTextureRect({{235, 83}, {188, 175}});
        }
        else
        {
            btnLeft.setTextureRect({{450, 82}, {187, 171}});
        }
        if (btnRight.getGlobalBounds().contains(mousePos))
        {
            btnRight.setTextureRect({{237, 119}, {186, 178}});
        }
        else
        {
            btnRight.setTextureRect({{27, 120}, {183, 176}});
        }
    }

    int getSelectedLevel(Vector2f mousePos)
    {
        if (frame.getGlobalBounds().contains(mousePos) && !isSliding)
        {
            return cards[currentIndex].levelID;
        }
        return -1;
    }

    void setButtonTextures(const Texture *left, const Texture *right)
    {
        btnLeft.setTexture(left);
        btnLeft.setTextureRect({{450, 82}, {187, 171}});
        btnRight.setTexture(right);
        btnRight.setTextureRect({{27, 120}, {183, 176}});
        btnLeft.setFillColor(Color::White);
        btnRight.setFillColor(Color::White);
    }
};