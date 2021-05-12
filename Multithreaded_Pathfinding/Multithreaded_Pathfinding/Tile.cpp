#include "Tile.h"

Tile::Tile()
{
}

Tile::~Tile()
{
}

void Tile::setTexture(std::string st)
{
}

void Tile::setPosition(sf::Vector2f pos)
{
	position = pos;
	m_tileSprite.setPosition(position);
}

sf::Vector2f Tile::getPosition()
{
	return position;
}

sf::Sprite* Tile::getSprite()
{
	return &m_tileSprite;
}

