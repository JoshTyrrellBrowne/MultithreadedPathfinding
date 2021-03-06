#include "Tile.h"

Tile::Tile(sf::Vector2f t_pos, float t_size)
{
	isGoal = false; // initially, used to indicate the goal
	isOnAPath = false; // initally false, not yet on a path
	opacityForPathView = 0; 
	position = t_pos * t_size;
	m_tileRect.setPosition(position);
	m_tileRect.setSize(sf::Vector2f(t_size, t_size));
	m_tileRect.setFillColor(sf::Color::Green);
	m_tileRect.setOutlineColor(sf::Color::Black);
	m_tileRect.setOutlineThickness(1);
}

Tile::~Tile()
{
}

void Tile::render(sf::RenderWindow* t_rendWind)
{
	t_rendWind->draw(m_tileRect);
}

void Tile::increaseOpacityForPathView()
{
	if (!isOnAPath) {   // this tile is not yet registered to any path
		isOnAPath = true;
		opacityForPathView += 50;
	}
	else {  // we know this tile is on a path, so now just bulk it up for next path entry
		if (opacityForPathView < 250)
		{
			opacityForPathView += 50;
		}
		else {
			opacityForPathView = 255;
		}
	}
}

void Tile::setColorPath()
{
	if (!isGoal)
	{
		m_tileRect.setFillColor(sf::Color(255,0,0, opacityForPathView));
	}
}

void Tile::setColour(sf::Color t_Colour)
{
	m_tileRect.setFillColor(t_Colour);
}

void Tile::setColourBlue()
{
	m_tileRect.setFillColor(sf::Color::Blue);
}

void Tile::setColourRed()
{
	m_tileRect.setFillColor(sf::Color::Red);
}

void Tile::setColourYellow()
{
	m_tileRect.setFillColor(sf::Color::Yellow);
}

void Tile::setColourBlack()
{
	m_tileRect.setFillColor(sf::Color::Black);
}

void Tile::setPosition(MyVector3 pos)
{
	//Handle from MyVector3 to sf::Vector2f
	position = sf::Vector2f(pos.x, pos.y);
	//position = position * m_tileRect.getGlobalBounds().width;  // not sure if this is correct, tough to test
	m_tileRect.setPosition(position);
}

sf::Vector2f Tile::getPosition()
{
	return position;
}


