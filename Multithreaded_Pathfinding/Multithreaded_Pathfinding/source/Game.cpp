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

// used for pathfinding only (so we can see path in console)
void visit(Node* t_node)
{
	//std::cout << "Visiting: " << t_node->m_data.m_position.x << ", " << t_node->m_data.m_position.y << std::endl;
}

void Game::GetPathToGoal(NPC* m_npcObj)
{
	threadID_Vec.at(m_npcObj->ID) = std::this_thread::get_id();

	Node* startNode = m_npcObj->getNodeFromPosition(m_npcObj->m_myTile->getPosition());
	Node* goalNode = m_npcObj->getNodeFromPosition(m_npcObj->m_goalPosition);
	m_npcObj->unMarkGraph();  // unmark all nodes so fresh graph
	m_npcObj->m_graph->aStar(m_npcObj->ID, startNode, goalNode, visit, m_npcObj->m_path);
}

bool Game::isPosOnImpassable(MyVector3 t_pos)
{
	for (MyVector3 pos : impassableTilPositions)
	{
		if (pos == t_pos)
		{
			return true;
		}
	}
	return false;
}
;

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
					m_npc->setGoalPosition(goalPosition);
					//m_npc->createThread(m_npc);
					m_thread_pool.addJob(std::bind(&Game::GetPathToGoal, this, m_npc));
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
	initImpassableTiles(); 

	m_tileSize = m_renderWin.getSize().y / mapSize;
	goalX = mapSize - 5;
	goalY = 5;
	goalPosition = sf::Vector2f(goalX * m_tileSize, goalY * m_tileSize);  // set goal position

	// Create the graph tile objects
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			Tile* tempTile = new Tile(sf::Vector2f(x, y), m_tileSize);
			if (x == goalX && y == goalY) // this tile is the goal
			{
				tempTile->setColourRed();
			}

			// check if impassable, color black if so
			for (MyVector3 pos : impassableTilPositions)
			{
				if (pos.x == (tempTile->getPosition().x / m_tileSize) && pos.y == (tempTile->getPosition().y / m_tileSize))
				{
					tempTile->setColourBlack();
				}
			}

			m_TileMap.push_back(*tempTile);
		}
	}

	initializeGraph();  // Create the node objects for the pathfinding graph
	// Init NPC's
	NPC* m_npc = nullptr;
	MyVector3 pos;
	for (int i = 0; i < m_npcCount; i++)
	{
		pos = MyVector3(rand() % mapSize, rand() % mapSize, 0);
		while (isPosOnImpassable(pos))
		{
			pos = MyVector3(rand() % mapSize, rand() % mapSize, 0);
		}


		m_npc = new NPC(i, sf::Vector2f(pos.x, pos.y), m_tileSize, graph);
		//m_npc = new NPC(i, sf::Vector2f((0 % mapSize), (0 % mapSize)), m_tileSize, graph);
		m_npcContainer.push_back(m_npc);
	}
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
	for (MyVector3 pos : impassableTilPositions)
	{
		for (int i = 0; i < graph->getNodes()->size(); i++)
		{
			if (pos.x == (graph->getNodes()->at(i)->m_data.m_position.x / m_tileSize) &&
				pos.y == (graph->getNodes()->at(i)->m_data.m_position.y / m_tileSize)) // this node position is impassable
			{
				graph->getNodes()->at(i)->m_data.m_isPassable = false;
				//m_circles.at(i)->setFillColor(sf::Color::Blue);
			}
		}
	}

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
				graph->addArc(i + (mapWidth * y), i + (mapWidth * y) + 1, m_tileSize);
			}
			// RIGHT COLUMN CATCH
			else if (i == (mapWidth - 1)) {
				graph->addArc(i + (mapWidth * y), i + (mapWidth * y) - 1, m_tileSize);
			}
			else
			{
				graph->addArc(i + (mapWidth * y), i + (mapWidth * y) + 1, m_tileSize);
				graph->addArc(i + (mapWidth * y), i + (mapWidth * y) - 1, m_tileSize);
			}

			// top and bottom neighbours========================================================
			// TOP ROW CATCH
			if (y == 0) { // TOP ROW
				graph->addArc(i + (mapWidth * (y + 1)), i + (mapWidth * y), m_tileSize);
			}
			//BOTTOM ROW CATCH
			else if (y == (mapHeight - 1))
			{
				graph->addArc(i + (mapWidth * (y - 1)), i + (mapWidth * y), m_tileSize);
			}
			else {
				graph->addArc(i + (mapWidth * (y - 1)), i + (mapWidth * y), m_tileSize);
				graph->addArc(i + (mapWidth * (y + 1)), i + (mapWidth * y), m_tileSize);
			}
		}
	}

	// Not needed anymore as we pass in to the constructor now
	/*for (auto m_npc : m_npcContainer)
	{
		m_npc->passGraph(graph);
	}*/
}

void Game::initImpassableTiles()
{
	impassableTilPositions.push_back(MyVector3(7, 0, 0));
	impassableTilPositions.push_back(MyVector3(7, 1, 0));
	impassableTilPositions.push_back(MyVector3(7, 2, 0));
	impassableTilPositions.push_back(MyVector3(7, 3, 0));
	impassableTilPositions.push_back(MyVector3(7, 4, 0));
	impassableTilPositions.push_back(MyVector3(7, 5, 0));
	impassableTilPositions.push_back(MyVector3(7, 6, 0));
	impassableTilPositions.push_back(MyVector3(7, 7, 0));
	impassableTilPositions.push_back(MyVector3(7, 8, 0));
	impassableTilPositions.push_back(MyVector3(7, 9, 0));
	impassableTilPositions.push_back(MyVector3(7, 10, 0));
	impassableTilPositions.push_back(MyVector3(7, 11, 0));
	impassableTilPositions.push_back(MyVector3(7, 12, 0));
	impassableTilPositions.push_back(MyVector3(7, 13, 0));
	impassableTilPositions.push_back(MyVector3(7, 14, 0));
	impassableTilPositions.push_back(MyVector3(7, 15, 0));
	impassableTilPositions.push_back(MyVector3(7, 16, 0));
	impassableTilPositions.push_back(MyVector3(7, 17, 0));
	impassableTilPositions.push_back(MyVector3(7, 18, 0));
	impassableTilPositions.push_back(MyVector3(7, 19, 0));
	impassableTilPositions.push_back(MyVector3(7, 20, 0));
	impassableTilPositions.push_back(MyVector3(7, 21, 0));
	impassableTilPositions.push_back(MyVector3(7, 22, 0));
	impassableTilPositions.push_back(MyVector3(7, 23, 0));
	impassableTilPositions.push_back(MyVector3(7, 24, 0));

	impassableTilPositions.push_back(MyVector3(14, 0, 0));
	impassableTilPositions.push_back(MyVector3(14, 1, 0));
	impassableTilPositions.push_back(MyVector3(14, 2, 0));
	impassableTilPositions.push_back(MyVector3(14, 3, 0));
	impassableTilPositions.push_back(MyVector3(14, 4, 0));
	impassableTilPositions.push_back(MyVector3(14, 5, 0));
	impassableTilPositions.push_back(MyVector3(14, 6, 0));
	impassableTilPositions.push_back(MyVector3(14, 7, 0));
	impassableTilPositions.push_back(MyVector3(14, 8, 0));
	impassableTilPositions.push_back(MyVector3(14, 9, 0));
	impassableTilPositions.push_back(MyVector3(14, 10, 0));
	impassableTilPositions.push_back(MyVector3(14, 11, 0));
	impassableTilPositions.push_back(MyVector3(14, 12, 0));
	impassableTilPositions.push_back(MyVector3(14, 13, 0));
	impassableTilPositions.push_back(MyVector3(14, 14, 0));
	impassableTilPositions.push_back(MyVector3(14, 15, 0));
	impassableTilPositions.push_back(MyVector3(14, 16, 0));
	impassableTilPositions.push_back(MyVector3(14, 17, 0));
	impassableTilPositions.push_back(MyVector3(14, 18, 0));
	impassableTilPositions.push_back(MyVector3(14, 19, 0));
	impassableTilPositions.push_back(MyVector3(14, 20, 0));
	impassableTilPositions.push_back(MyVector3(14, 21, 0));
	impassableTilPositions.push_back(MyVector3(14, 22, 0));
	impassableTilPositions.push_back(MyVector3(14, 23, 0));
	impassableTilPositions.push_back(MyVector3(14, 24, 0));

	impassableTilPositions.push_back(MyVector3(21, 0, 0));
	impassableTilPositions.push_back(MyVector3(21, 1, 0));
	impassableTilPositions.push_back(MyVector3(21, 2, 0));
	impassableTilPositions.push_back(MyVector3(21, 3, 0));
	impassableTilPositions.push_back(MyVector3(21, 4, 0));
	impassableTilPositions.push_back(MyVector3(21, 5, 0));
	impassableTilPositions.push_back(MyVector3(21, 6, 0));
	impassableTilPositions.push_back(MyVector3(21, 7, 0));
	impassableTilPositions.push_back(MyVector3(21, 8, 0));
	impassableTilPositions.push_back(MyVector3(21, 9, 0));
	impassableTilPositions.push_back(MyVector3(21, 10, 0));
	impassableTilPositions.push_back(MyVector3(21, 11, 0));
	impassableTilPositions.push_back(MyVector3(21, 12, 0));
	impassableTilPositions.push_back(MyVector3(21, 13, 0));
	impassableTilPositions.push_back(MyVector3(21, 14, 0));
	impassableTilPositions.push_back(MyVector3(21, 15, 0));
	impassableTilPositions.push_back(MyVector3(21, 16, 0));
	impassableTilPositions.push_back(MyVector3(21, 17, 0));
	impassableTilPositions.push_back(MyVector3(21, 18, 0));
	impassableTilPositions.push_back(MyVector3(21, 19, 0));
	impassableTilPositions.push_back(MyVector3(21, 20, 0));
	impassableTilPositions.push_back(MyVector3(21, 21, 0));
	impassableTilPositions.push_back(MyVector3(21, 22, 0));
	impassableTilPositions.push_back(MyVector3(21, 23, 0));
	impassableTilPositions.push_back(MyVector3(21, 24, 0));

}
