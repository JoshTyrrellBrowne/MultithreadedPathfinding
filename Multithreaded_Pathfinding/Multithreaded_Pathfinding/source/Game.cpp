#include "Game.h"

// Made by: Josh Browne

// Function for sorting graph nodes in right order (left -> right, Top -> down)
bool sortNodes(const Node* n1, const Node* n2)
{
	if (n1->m_data.m_position.y == n2->m_data.m_position.y)
	{
		return n1->m_data.m_position.x < n2->m_data.m_position.x;
	}
	else
	{
		return n1->m_data.m_position.y < n2->m_data.m_position.y;
	}
}

// Contructor
Game::Game() :
	m_renderWin{ sf::VideoMode{ 1920, 1280, 1 }, "MultiThreaded Pathfinding"}
{
	srand(time(nullptr));
	intialize();
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
			if (sf::Keyboard::Space == event.key.code)
			{
				for (auto m_npc : m_npcContainer)
				{
					m_npc->setGoalPosition(sf::Vector2f(980, 980));
					m_npc->createThread(m_npc);
				}
			}
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
	for (auto m_npc : m_npcContainer)
	{
		m_npc->update(t_deltaTime);
	}
}

// Renders
void Game::render()
{
	m_renderWin.clear(sf::Color::Black);
	for (int i = 0; i < m_TileMap.size(); i++)
	{
		m_TileMap.at(i).render(&m_renderWin);
	}
	for (auto m_npc : m_npcContainer)
	{
		m_npc->render(&m_renderWin);
	}
	
	m_renderWin.display();
}


// Everything is initialised from here
void Game::intialize()
{
	m_tileSize = 20;
	// Init NPC's
	NPC* m_npc = nullptr;
	for (int i = 0; i < m_npcCount; i++)
	{
		m_npc = new NPC(sf::Vector2f((rand() % 50), (rand() % 50)), m_tileSize);
		m_npcContainer.push_back(m_npc);
	}
	

	// Create the graph tile objects
	for (int x = 0; x < 50; x++)
	{
		for (int y = 0; y < 50; y++)
		{
			Tile* tempTile = new Tile(sf::Vector2f(x, y), m_tileSize);
			m_TileMap.push_back(*tempTile);
		}
	}

	initializeGraph();  // Create the node objects for the pathfinding graph
}

void Game::initializeGraph()
{
	delete graph; // delete from heap
	graph = new Graph<NodeData, int>(m_TileMap.size());// m_bottomLayer.size());

	NodeData nodeData; // needed for graph populating

	int index = 0;
	for (Tile tile : m_TileMap)
	{
		nodeData.m_position = tile.getPosition();
		nodeData.m_isPassable = true;
		graph->addNode(nodeData, index);
		index++;
	}

	// Maybe use below for wallzzz ye

	//for (Tile* tile : m_level)
	//{
	//	for (int i = 0; i < graph->getNodes()->size(); i++)
	//	{
	//		if ((MyVector3)tile->getPosition() == graph->getNodes()->at(i)->m_data.m_position) // this node position already exists
	//		{
	//			graph->getNodes()->at(i)->m_data.m_isPassable = false;
	//			m_circles.at(i)->setFillColor(sf::Color::Blue);
	//		}
	//	}
	//}

	std::sort(graph->getNodes()->begin(), graph->getNodes()->end(), sortNodes); // sort the nodes to be in correct order
	setGraphNeighbours(); // Set the neighbours
}

void Game::setGraphNeighbours()
{
	int mapWidth = 50;
	int mapHeight = 50;

	for (int y = 0; y < mapHeight; y++)
	{
		for (int i = 0; i < mapWidth; i++)
		{
			// left and right neighbours======================================================
			//LEFT COLUMMN CATCH
			if (i == 0) { // THE LEFT COLUMN
				graph->addArc(i + (mapWidth * y), i + (mapWidth * y) + 1, 64);
			}
			// RIGHT COLUMN CATCH
			else if (i == (mapWidth - 1)) {
				graph->addArc(i + (mapWidth * y), i + (mapWidth * y) - 1, 64);
			}
			else
			{
				graph->addArc(i + (mapWidth * y), i + (mapWidth * y) + 1, 64);
				graph->addArc(i + (mapWidth * y), i + (mapWidth * y) - 1, 64);
			}

			// top and bottom neighbours========================================================
			// TOP ROW CATCH
			if (y == 0) { // TOP ROW
				graph->addArc(i + (mapWidth * (y + 1)), i + (mapWidth * y), 64);
			}
			//BOTTOM ROW CATCH
			else if (y == (mapHeight - 1))
			{
				graph->addArc(i + (mapWidth * (y - 1)), i + (mapWidth * y), 64);
			}
			else {
				graph->addArc(i + (mapWidth * (y - 1)), i + (mapWidth * y), 64);
				graph->addArc(i + (mapWidth * (y + 1)), i + (mapWidth * y), 64);
			}
		}
	}
	for (auto m_npc : m_npcContainer)
	{
		m_npc->passGraph(graph);
	}
}
