#include "Game.h"

// Made by: Josh Browne

// Contructor
Game::Game() :
	m_renderWin{ sf::VideoMode{ 1920, 1280, 1 }, "MultiThreaded Pathfinding"}
{
	intialize();
	srand(time(nullptr));
}

/// Destructor
Game::~Game()
{
	
}

// Loop designed to work at equal speed on all PCs
// If a PC is slower, it will update the same amount of times
// And render less often
void Game::run()
{
	sf::Clock gameClock;											// Game clock
	sf::Time timeTakenForUpdate = sf::Time::Zero;					// No lag on first input
	sf::Time timePerFrame = sf::seconds(1.f / 60.f);				// 60 frames per second
	while (m_renderWin.isOpen())									// Loop
	{
		processInput();												// Check for input
		timeTakenForUpdate += gameClock.restart();					// Returns time take to do the loop
		while (timeTakenForUpdate > timePerFrame)					// Update enough times to catch up on updates missed during the lag time
		{
			timeTakenForUpdate -= timePerFrame;						// Decrement lag
			processInput();											// More checks, the more accurate input to display will be
			update(timePerFrame);									// Update
		}
		render();
	}
}

// Process PC input
void Game::processInput()
{
	sf::Event event;
	while (m_renderWin.pollEvent(event))
	{
		if (sf::Event::Closed == event.type)
		{
			m_renderWin.close();
		}
		if (sf::Event::KeyPressed == event.type)
		{
			if (sf::Keyboard::Escape == event.key.code)
			{
				m_renderWin.close();
			}
		}
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{

			}
			if (event.mouseButton.button == sf::Mouse::Right)
			{

			}
		}
		if (event.type == sf::Event::MouseButtonReleased)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
			}
			if (event.mouseButton.button == sf::Mouse::Right)
			{
			}
		}
		if (event.type == sf::Event::MouseWheelMoved)
		{
			// display number of ticks mouse wheel has moved
			//m_scroll = event.mouseWheel.delta;
			//std::cout << event.mouseWheel.delta << '\n';
		}
	}

}

// Updates Game
void Game::update(sf::Time t_deltaTime)
{
	
}

// Renders
void Game::render()
{
	m_renderWin.clear(sf::Color::Black);
	
	
	m_renderWin.display();
}

void Game::intialize()
{

}
