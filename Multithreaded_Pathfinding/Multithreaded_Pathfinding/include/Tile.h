#pragma once

#include <iostream>
#include <string>
#include "SFML/Graphics.hpp"
#include "MyVector3.h"


class Tile
{
public:
	Tile(sf::Vector2f t_pos, float t_size);
	~Tile();
	void render(sf::RenderWindow* t_rendWind);

	void setColour(sf::Color t_Colour);
	void setColourBlue();
	void setColourRed();
	void setColourBlack();
	void setPosition(MyVector3 pos);
	sf::Vector2f getPosition();


private:
	sf::RectangleShape m_tileRect;
	sf::Vector2f position;
};

