#pragma once

#include <iostream>
#include <string>
#include "SFML/Graphics.hpp"



class Tile
{
public:
	Tile();
	~Tile();
	void setTexture(std::string st);
	void setPosition(sf::Vector2f pos);
	sf::Vector2f getPosition();
	sf::Sprite* getSprite();

	sf::Vector2f position;
private:
	sf::Sprite m_tileSprite;
	
};

