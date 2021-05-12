#pragma once

// Made by: Robin Meyler, Josh Browne

// Extra ----------------------------
#include <iostream>
#include "SFML/Graphics.hpp"


class Game
{
public:
	Game();
	~Game();
	void run();

protected:
	void intialize();
	void update(sf::Time t_deltaTime);
	void processInput();
	void render();


private:
	sf::RenderWindow m_renderWin; // main SFML window
};

