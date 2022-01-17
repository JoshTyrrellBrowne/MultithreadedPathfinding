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
	std::cout << "Press the Space bar to initiate Pathfinding" << std::endl;
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
	//std::cout << "Thread: " << std::this_thread::get_id() << " is now visiting: " << t_node->m_data.m_position.x << ", " << t_node->m_data.m_position.y << std::endl;
}

void Game::GetPathToGoal(NPC* m_npcObj)
{
	threadID_Vec.at(m_npcObj->ID) = std::this_thread::get_id();

	Node* startNode = m_npcObj->getNodeFromPosition(m_npcObj->m_myTile->getPosition());
	Node* goalNode = m_npcObj->getNodeFromPosition(m_npcObj->m_goalPosition);
	m_npcObj->unMarkGraph();  // unmark all nodes so fresh graph
	m_npcObj->m_graph->aStar(m_npcObj->ID, startNode, goalNode, visit, m_npcObj->m_path);

	threadID_Vec.at(m_npcObj->ID) = std::thread::id();
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
				std::cout << "Beginning A*mbush pathfinding" << std::endl;
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
		//pos = MyVector3(0, 0, 0);
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
	int mapWidth = 100;
	int mapHeight = 100;

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
	//impassableTilPositions.push_back(MyVector3(7, 8, 0));
	//impassableTilPositions.push_back(MyVector3(7, 9, 0));
	//impassableTilPositions.push_back(MyVector3(7, 10, 0));
	//impassableTilPositions.push_back(MyVector3(7, 11, 0));
	//impassableTilPositions.push_back(MyVector3(7, 12, 0));
	//impassableTilPositions.push_back(MyVector3(7, 13, 0));
	//impassableTilPositions.push_back(MyVector3(7, 14, 0));
	//impassableTilPositions.push_back(MyVector3(7, 15, 0));
	impassableTilPositions.push_back(MyVector3(7, 16, 0));
	impassableTilPositions.push_back(MyVector3(7, 17, 0));
	impassableTilPositions.push_back(MyVector3(7, 18, 0));
	impassableTilPositions.push_back(MyVector3(7, 19, 0));
	impassableTilPositions.push_back(MyVector3(7, 20, 0));
	impassableTilPositions.push_back(MyVector3(7, 21, 0));
	impassableTilPositions.push_back(MyVector3(7, 22, 0));
	impassableTilPositions.push_back(MyVector3(7, 23, 0));
	impassableTilPositions.push_back(MyVector3(7, 24, 0));
	impassableTilPositions.push_back(MyVector3(7, 25, 0));
	impassableTilPositions.push_back(MyVector3(7, 26, 0));
	impassableTilPositions.push_back(MyVector3(7, 27, 0));
	impassableTilPositions.push_back(MyVector3(7, 28, 0));
	impassableTilPositions.push_back(MyVector3(7, 29, 0));
	impassableTilPositions.push_back(MyVector3(7, 30, 0));
	impassableTilPositions.push_back(MyVector3(7, 31, 0));
	impassableTilPositions.push_back(MyVector3(7, 32, 0));
	impassableTilPositions.push_back(MyVector3(7, 33, 0));
	impassableTilPositions.push_back(MyVector3(7, 34, 0));
	impassableTilPositions.push_back(MyVector3(7, 35, 0));
	impassableTilPositions.push_back(MyVector3(7, 36, 0));
	impassableTilPositions.push_back(MyVector3(7, 37, 0));
	impassableTilPositions.push_back(MyVector3(7, 38, 0));
	impassableTilPositions.push_back(MyVector3(7, 39, 0));
	impassableTilPositions.push_back(MyVector3(7, 40, 0));
	impassableTilPositions.push_back(MyVector3(7, 41, 0));
	impassableTilPositions.push_back(MyVector3(7, 42, 0));
	impassableTilPositions.push_back(MyVector3(7, 43, 0));
	impassableTilPositions.push_back(MyVector3(7, 44, 0));
	impassableTilPositions.push_back(MyVector3(7, 45, 0));
	impassableTilPositions.push_back(MyVector3(7, 46, 0));
	impassableTilPositions.push_back(MyVector3(7, 47, 0));
	impassableTilPositions.push_back(MyVector3(7, 48, 0));
	impassableTilPositions.push_back(MyVector3(7, 49, 0));
	impassableTilPositions.push_back(MyVector3(7, 50, 0));
	impassableTilPositions.push_back(MyVector3(7, 51, 0));
	impassableTilPositions.push_back(MyVector3(7, 52, 0));
	impassableTilPositions.push_back(MyVector3(7, 53, 0));
	impassableTilPositions.push_back(MyVector3(7, 54, 0));
	impassableTilPositions.push_back(MyVector3(7, 55, 0));
	impassableTilPositions.push_back(MyVector3(7, 56, 0));
	impassableTilPositions.push_back(MyVector3(7, 57, 0));
	impassableTilPositions.push_back(MyVector3(7, 58, 0));
	impassableTilPositions.push_back(MyVector3(7, 59, 0));
	impassableTilPositions.push_back(MyVector3(7, 60, 0));
	impassableTilPositions.push_back(MyVector3(7, 61, 0));
	impassableTilPositions.push_back(MyVector3(7, 62, 0));
	impassableTilPositions.push_back(MyVector3(7, 63, 0));
	impassableTilPositions.push_back(MyVector3(7, 64, 0));
	impassableTilPositions.push_back(MyVector3(7, 65, 0));
	impassableTilPositions.push_back(MyVector3(7, 66, 0));
	impassableTilPositions.push_back(MyVector3(7, 67, 0));
	impassableTilPositions.push_back(MyVector3(7, 68, 0));
	impassableTilPositions.push_back(MyVector3(7, 69, 0));
	impassableTilPositions.push_back(MyVector3(7, 70, 0));
	impassableTilPositions.push_back(MyVector3(7, 71, 0));
	impassableTilPositions.push_back(MyVector3(7, 72, 0));
	impassableTilPositions.push_back(MyVector3(7, 73, 0));
	impassableTilPositions.push_back(MyVector3(7, 74, 0));
	//impassableTilPositions.push_back(MyVector3(7, 75, 0));
	//impassableTilPositions.push_back(MyVector3(7, 76, 0));
	//impassableTilPositions.push_back(MyVector3(7, 77, 0));
	//impassableTilPositions.push_back(MyVector3(7, 78, 0));
	//impassableTilPositions.push_back(MyVector3(7, 79, 0));
	//impassableTilPositions.push_back(MyVector3(7, 80, 0));
	//impassableTilPositions.push_back(MyVector3(7, 81, 0));
	//impassableTilPositions.push_back(MyVector3(7, 82, 0));
	//impassableTilPositions.push_back(MyVector3(7, 83, 0));
	//impassableTilPositions.push_back(MyVector3(7, 84, 0));
	//impassableTilPositions.push_back(MyVector3(7, 85, 0));
	//impassableTilPositions.push_back(MyVector3(7, 86, 0));
	//impassableTilPositions.push_back(MyVector3(7, 87, 0));
	//impassableTilPositions.push_back(MyVector3(7, 88, 0));
	impassableTilPositions.push_back(MyVector3(7, 89, 0));
	impassableTilPositions.push_back(MyVector3(7, 90, 0));
	impassableTilPositions.push_back(MyVector3(7, 91, 0));
	impassableTilPositions.push_back(MyVector3(7, 92, 0));
	impassableTilPositions.push_back(MyVector3(7, 93, 0));
	impassableTilPositions.push_back(MyVector3(7, 94, 0));
	impassableTilPositions.push_back(MyVector3(7, 95, 0));
	impassableTilPositions.push_back(MyVector3(7, 96, 0));
	impassableTilPositions.push_back(MyVector3(7, 97, 0));
	impassableTilPositions.push_back(MyVector3(7, 98, 0));
	impassableTilPositions.push_back(MyVector3(7, 99, 0));

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
	//impassableTilPositions.push_back(MyVector3(14, 17, 0));
	//impassableTilPositions.push_back(MyVector3(14, 18, 0));
	//impassableTilPositions.push_back(MyVector3(14, 19, 0));
	//impassableTilPositions.push_back(MyVector3(14, 20, 0));
	//impassableTilPositions.push_back(MyVector3(14, 21, 0));
	//impassableTilPositions.push_back(MyVector3(14, 22, 0));
	//impassableTilPositions.push_back(MyVector3(14, 23, 0));
	//impassableTilPositions.push_back(MyVector3(14, 24, 0));
	//impassableTilPositions.push_back(MyVector3(14, 25, 0));
	//impassableTilPositions.push_back(MyVector3(14, 26, 0));
	//impassableTilPositions.push_back(MyVector3(14, 27, 0));
	//impassableTilPositions.push_back(MyVector3(14, 28, 0));
	//impassableTilPositions.push_back(MyVector3(14, 29, 0));
	//impassableTilPositions.push_back(MyVector3(14, 30, 0));
	//impassableTilPositions.push_back(MyVector3(14, 31, 0));
	//impassableTilPositions.push_back(MyVector3(14, 32, 0));
	//impassableTilPositions.push_back(MyVector3(14, 33, 0));
	//impassableTilPositions.push_back(MyVector3(14, 34, 0));
	//impassableTilPositions.push_back(MyVector3(14, 35, 0));
	//impassableTilPositions.push_back(MyVector3(14, 36, 0));
	//impassableTilPositions.push_back(MyVector3(14, 37, 0));
	//impassableTilPositions.push_back(MyVector3(14, 38, 0));
	//impassableTilPositions.push_back(MyVector3(14, 39, 0));
	//impassableTilPositions.push_back(MyVector3(14, 40, 0));
	//impassableTilPositions.push_back(MyVector3(14, 41, 0));
	//impassableTilPositions.push_back(MyVector3(14, 42, 0));
	//impassableTilPositions.push_back(MyVector3(14, 43, 0));
	//impassableTilPositions.push_back(MyVector3(14, 44, 0));
	//impassableTilPositions.push_back(MyVector3(14, 45, 0));
	//impassableTilPositions.push_back(MyVector3(14, 46, 0));
	//impassableTilPositions.push_back(MyVector3(14, 47, 0));
	//impassableTilPositions.push_back(MyVector3(14, 48, 0));
	//impassableTilPositions.push_back(MyVector3(14, 49, 0));
	//impassableTilPositions.push_back(MyVector3(14, 50, 0));
	//impassableTilPositions.push_back(MyVector3(14, 51, 0));
	//impassableTilPositions.push_back(MyVector3(14, 52, 0));
	//impassableTilPositions.push_back(MyVector3(14, 53, 0));
	//impassableTilPositions.push_back(MyVector3(14, 54, 0));
	//impassableTilPositions.push_back(MyVector3(14, 55, 0));
	//impassableTilPositions.push_back(MyVector3(14, 56, 0));
	//impassableTilPositions.push_back(MyVector3(14, 57, 0));
	//impassableTilPositions.push_back(MyVector3(14, 58, 0));
	//impassableTilPositions.push_back(MyVector3(14, 59, 0));
	//impassableTilPositions.push_back(MyVector3(14, 60, 0));
	//impassableTilPositions.push_back(MyVector3(14, 61, 0));
	//impassableTilPositions.push_back(MyVector3(14, 62, 0));
	//impassableTilPositions.push_back(MyVector3(14, 63, 0));
	//impassableTilPositions.push_back(MyVector3(14, 64, 0));
	//impassableTilPositions.push_back(MyVector3(14, 65, 0));
	//impassableTilPositions.push_back(MyVector3(14, 66, 0));
	//impassableTilPositions.push_back(MyVector3(14, 67, 0));
	//impassableTilPositions.push_back(MyVector3(14, 68, 0));
	//impassableTilPositions.push_back(MyVector3(14, 69, 0));
	//impassableTilPositions.push_back(MyVector3(14, 70, 0));
	//impassableTilPositions.push_back(MyVector3(14, 71, 0));
	impassableTilPositions.push_back(MyVector3(14, 72, 0));
	impassableTilPositions.push_back(MyVector3(14, 73, 0));
	impassableTilPositions.push_back(MyVector3(14, 74, 0));
	impassableTilPositions.push_back(MyVector3(14, 75, 0));
	impassableTilPositions.push_back(MyVector3(14, 76, 0));
	impassableTilPositions.push_back(MyVector3(14, 77, 0));
	impassableTilPositions.push_back(MyVector3(14, 78, 0));
	impassableTilPositions.push_back(MyVector3(14, 79, 0));
	impassableTilPositions.push_back(MyVector3(14, 80, 0));
	impassableTilPositions.push_back(MyVector3(14, 81, 0));
	impassableTilPositions.push_back(MyVector3(14, 82, 0));
	impassableTilPositions.push_back(MyVector3(14, 83, 0));
	impassableTilPositions.push_back(MyVector3(14, 84, 0));
	impassableTilPositions.push_back(MyVector3(14, 85, 0));
	impassableTilPositions.push_back(MyVector3(14, 86, 0));
	impassableTilPositions.push_back(MyVector3(14, 87, 0));
	impassableTilPositions.push_back(MyVector3(14, 88, 0));
	impassableTilPositions.push_back(MyVector3(14, 89, 0));
	impassableTilPositions.push_back(MyVector3(14, 90, 0));
	impassableTilPositions.push_back(MyVector3(14, 91, 0));
	impassableTilPositions.push_back(MyVector3(14, 92, 0));
	impassableTilPositions.push_back(MyVector3(14, 93, 0));
	impassableTilPositions.push_back(MyVector3(14, 94, 0));
	impassableTilPositions.push_back(MyVector3(14, 95, 0));
	impassableTilPositions.push_back(MyVector3(14, 96, 0));
	impassableTilPositions.push_back(MyVector3(14, 97, 0));
	impassableTilPositions.push_back(MyVector3(14, 98, 0));
	impassableTilPositions.push_back(MyVector3(14, 99, 0));

	//impassableTilPositions.push_back(MyVector3(21, 0, 0));
	//impassableTilPositions.push_back(MyVector3(21, 1, 0));
	//impassableTilPositions.push_back(MyVector3(21, 2, 0));
	//impassableTilPositions.push_back(MyVector3(21, 3, 0));
	//impassableTilPositions.push_back(MyVector3(21, 4, 0));
	//impassableTilPositions.push_back(MyVector3(21, 5, 0));
	//impassableTilPositions.push_back(MyVector3(21, 6, 0));
	//impassableTilPositions.push_back(MyVector3(21, 7, 0));
	//impassableTilPositions.push_back(MyVector3(21, 8, 0));
	//impassableTilPositions.push_back(MyVector3(21, 9, 0));
	//impassableTilPositions.push_back(MyVector3(21, 10, 0));
	//impassableTilPositions.push_back(MyVector3(21, 11, 0));
	//impassableTilPositions.push_back(MyVector3(21, 12, 0));
	//impassableTilPositions.push_back(MyVector3(21, 13, 0));
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
	impassableTilPositions.push_back(MyVector3(21, 25, 0));
	impassableTilPositions.push_back(MyVector3(21, 26, 0));
	impassableTilPositions.push_back(MyVector3(21, 27, 0));
	impassableTilPositions.push_back(MyVector3(21, 28, 0));
	impassableTilPositions.push_back(MyVector3(21, 29, 0));
	impassableTilPositions.push_back(MyVector3(21, 30, 0));
	impassableTilPositions.push_back(MyVector3(21, 31, 0));
	impassableTilPositions.push_back(MyVector3(21, 32, 0));
	impassableTilPositions.push_back(MyVector3(21, 33, 0));
	impassableTilPositions.push_back(MyVector3(21, 34, 0));
	impassableTilPositions.push_back(MyVector3(21, 35, 0));
	impassableTilPositions.push_back(MyVector3(21, 36, 0));
	impassableTilPositions.push_back(MyVector3(21, 37, 0));
	impassableTilPositions.push_back(MyVector3(21, 38, 0));
	impassableTilPositions.push_back(MyVector3(21, 39, 0));
	impassableTilPositions.push_back(MyVector3(21, 40, 0));
	impassableTilPositions.push_back(MyVector3(21, 41, 0));
	impassableTilPositions.push_back(MyVector3(21, 42, 0));
	impassableTilPositions.push_back(MyVector3(21, 43, 0));
	impassableTilPositions.push_back(MyVector3(21, 44, 0));
	impassableTilPositions.push_back(MyVector3(21, 45, 0));
	impassableTilPositions.push_back(MyVector3(21, 46, 0));
	impassableTilPositions.push_back(MyVector3(21, 47, 0));
	impassableTilPositions.push_back(MyVector3(21, 48, 0));
	impassableTilPositions.push_back(MyVector3(21, 49, 0));
	impassableTilPositions.push_back(MyVector3(21, 50, 0));
	impassableTilPositions.push_back(MyVector3(21, 51, 0));
	impassableTilPositions.push_back(MyVector3(21, 52, 0));
	impassableTilPositions.push_back(MyVector3(21, 53, 0));
	impassableTilPositions.push_back(MyVector3(21, 54, 0));
	impassableTilPositions.push_back(MyVector3(21, 55, 0));
	impassableTilPositions.push_back(MyVector3(21, 56, 0));
	impassableTilPositions.push_back(MyVector3(21, 57, 0));
	impassableTilPositions.push_back(MyVector3(21, 58, 0));
	impassableTilPositions.push_back(MyVector3(21, 59, 0));
	impassableTilPositions.push_back(MyVector3(21, 60, 0));
	impassableTilPositions.push_back(MyVector3(21, 61, 0));
	impassableTilPositions.push_back(MyVector3(21, 62, 0));
	impassableTilPositions.push_back(MyVector3(21, 63, 0));
	impassableTilPositions.push_back(MyVector3(21, 64, 0));
	impassableTilPositions.push_back(MyVector3(21, 65, 0));
	impassableTilPositions.push_back(MyVector3(21, 66, 0));
	impassableTilPositions.push_back(MyVector3(21, 67, 0));
	impassableTilPositions.push_back(MyVector3(21, 68, 0));
	impassableTilPositions.push_back(MyVector3(21, 69, 0));
	impassableTilPositions.push_back(MyVector3(21, 70, 0));
	impassableTilPositions.push_back(MyVector3(21, 71, 0));
	impassableTilPositions.push_back(MyVector3(21, 72, 0));
	impassableTilPositions.push_back(MyVector3(21, 73, 0));
	impassableTilPositions.push_back(MyVector3(21, 74, 0));
	impassableTilPositions.push_back(MyVector3(21, 75, 0));
	impassableTilPositions.push_back(MyVector3(21, 76, 0));
	impassableTilPositions.push_back(MyVector3(21, 77, 0));
	impassableTilPositions.push_back(MyVector3(21, 78, 0));
	impassableTilPositions.push_back(MyVector3(21, 79, 0));
	//impassableTilPositions.push_back(MyVector3(21, 80, 0));
	//impassableTilPositions.push_back(MyVector3(21, 81, 0));
	//impassableTilPositions.push_back(MyVector3(21, 82, 0));
	//impassableTilPositions.push_back(MyVector3(21, 83, 0));
	//impassableTilPositions.push_back(MyVector3(21, 84, 0));
	//impassableTilPositions.push_back(MyVector3(21, 85, 0));
	//impassableTilPositions.push_back(MyVector3(21, 86, 0));
	//impassableTilPositions.push_back(MyVector3(21, 87, 0));
	//impassableTilPositions.push_back(MyVector3(21, 88, 0));
	//impassableTilPositions.push_back(MyVector3(21, 89, 0));
	//impassableTilPositions.push_back(MyVector3(21, 90, 0));
	//impassableTilPositions.push_back(MyVector3(21, 91, 0));
	//impassableTilPositions.push_back(MyVector3(21, 92, 0));
	//impassableTilPositions.push_back(MyVector3(21, 93, 0));
	//impassableTilPositions.push_back(MyVector3(21, 94, 0));
	//impassableTilPositions.push_back(MyVector3(21, 95, 0));
	//impassableTilPositions.push_back(MyVector3(21, 96, 0));
	//impassableTilPositions.push_back(MyVector3(21, 97, 0));
	//impassableTilPositions.push_back(MyVector3(21, 98, 0));
	//impassableTilPositions.push_back(MyVector3(21, 99, 0));

	impassableTilPositions.push_back(MyVector3(28, 0, 0));
	impassableTilPositions.push_back(MyVector3(28, 1, 0));
	impassableTilPositions.push_back(MyVector3(28, 2, 0));
	impassableTilPositions.push_back(MyVector3(28, 3, 0));
	impassableTilPositions.push_back(MyVector3(28, 4, 0));
	impassableTilPositions.push_back(MyVector3(28, 5, 0));
	impassableTilPositions.push_back(MyVector3(28, 6, 0));
	impassableTilPositions.push_back(MyVector3(28, 7, 0));
	impassableTilPositions.push_back(MyVector3(28, 8, 0));
	impassableTilPositions.push_back(MyVector3(28, 9, 0));
	impassableTilPositions.push_back(MyVector3(28, 10, 0));
	impassableTilPositions.push_back(MyVector3(28, 11, 0));
	impassableTilPositions.push_back(MyVector3(28, 12, 0));
	impassableTilPositions.push_back(MyVector3(28, 13, 0));
	impassableTilPositions.push_back(MyVector3(28, 14, 0));
	impassableTilPositions.push_back(MyVector3(28, 15, 0));
	impassableTilPositions.push_back(MyVector3(28, 16, 0));
	impassableTilPositions.push_back(MyVector3(28, 17, 0));
	impassableTilPositions.push_back(MyVector3(28, 18, 0));
	impassableTilPositions.push_back(MyVector3(28, 19, 0));
	impassableTilPositions.push_back(MyVector3(28, 20, 0));
	impassableTilPositions.push_back(MyVector3(28, 21, 0));
	impassableTilPositions.push_back(MyVector3(28, 22, 0));
	impassableTilPositions.push_back(MyVector3(28, 23, 0));
	impassableTilPositions.push_back(MyVector3(28, 24, 0));
	impassableTilPositions.push_back(MyVector3(28, 25, 0));
	impassableTilPositions.push_back(MyVector3(28, 26, 0));
	impassableTilPositions.push_back(MyVector3(28, 27, 0));
	impassableTilPositions.push_back(MyVector3(28, 28, 0));
	impassableTilPositions.push_back(MyVector3(28, 29, 0));
	impassableTilPositions.push_back(MyVector3(28, 30, 0));
	impassableTilPositions.push_back(MyVector3(28, 31, 0));
	impassableTilPositions.push_back(MyVector3(28, 32, 0));
	impassableTilPositions.push_back(MyVector3(28, 33, 0));
	impassableTilPositions.push_back(MyVector3(28, 34, 0));
	impassableTilPositions.push_back(MyVector3(28, 35, 0));
	impassableTilPositions.push_back(MyVector3(28, 36, 0));
	impassableTilPositions.push_back(MyVector3(28, 37, 0));
	impassableTilPositions.push_back(MyVector3(28, 38, 0));
	impassableTilPositions.push_back(MyVector3(28, 39, 0));
	//impassableTilPositions.push_back(MyVector3(28, 40, 0));
	//impassableTilPositions.push_back(MyVector3(28, 41, 0));
	//impassableTilPositions.push_back(MyVector3(28, 42, 0));
	//impassableTilPositions.push_back(MyVector3(28, 43, 0));
	//impassableTilPositions.push_back(MyVector3(28, 44, 0));
	//impassableTilPositions.push_back(MyVector3(28, 45, 0));
	//impassableTilPositions.push_back(MyVector3(28, 46, 0));
	//impassableTilPositions.push_back(MyVector3(28, 47, 0));
	//impassableTilPositions.push_back(MyVector3(28, 48, 0));
	//impassableTilPositions.push_back(MyVector3(28, 49, 0));
	//impassableTilPositions.push_back(MyVector3(28, 50, 0));
	//impassableTilPositions.push_back(MyVector3(28, 51, 0));
	//impassableTilPositions.push_back(MyVector3(28, 52, 0));
	//impassableTilPositions.push_back(MyVector3(28, 53, 0));
	//impassableTilPositions.push_back(MyVector3(28, 54, 0));
	//impassableTilPositions.push_back(MyVector3(28, 55, 0));
	//impassableTilPositions.push_back(MyVector3(28, 56, 0));
	//impassableTilPositions.push_back(MyVector3(28, 57, 0));
	//impassableTilPositions.push_back(MyVector3(28, 58, 0));
	//impassableTilPositions.push_back(MyVector3(28, 59, 0));
	//impassableTilPositions.push_back(MyVector3(28, 60, 0));
	//impassableTilPositions.push_back(MyVector3(28, 61, 0));
	//impassableTilPositions.push_back(MyVector3(28, 62, 0));
	//impassableTilPositions.push_back(MyVector3(28, 63, 0));
	//impassableTilPositions.push_back(MyVector3(28, 64, 0));
	//impassableTilPositions.push_back(MyVector3(28, 65, 0));
	//impassableTilPositions.push_back(MyVector3(28, 66, 0));
	//impassableTilPositions.push_back(MyVector3(28, 67, 0));
	//impassableTilPositions.push_back(MyVector3(28, 68, 0));
	//impassableTilPositions.push_back(MyVector3(28, 69, 0));
	//impassableTilPositions.push_back(MyVector3(28, 70, 0));
	//impassableTilPositions.push_back(MyVector3(28, 71, 0));
	//impassableTilPositions.push_back(MyVector3(28, 72, 0));
	//impassableTilPositions.push_back(MyVector3(28, 73, 0));
	//impassableTilPositions.push_back(MyVector3(28, 74, 0));
	//impassableTilPositions.push_back(MyVector3(28, 75, 0));
	//impassableTilPositions.push_back(MyVector3(28, 76, 0));
	impassableTilPositions.push_back(MyVector3(28, 77, 0));
	impassableTilPositions.push_back(MyVector3(28, 78, 0));
	impassableTilPositions.push_back(MyVector3(28, 79, 0));
	impassableTilPositions.push_back(MyVector3(28, 80, 0));
	impassableTilPositions.push_back(MyVector3(28, 81, 0));
	impassableTilPositions.push_back(MyVector3(28, 82, 0));
	impassableTilPositions.push_back(MyVector3(28, 83, 0));
	impassableTilPositions.push_back(MyVector3(28, 84, 0));
	impassableTilPositions.push_back(MyVector3(28, 85, 0));
	impassableTilPositions.push_back(MyVector3(28, 86, 0));
	impassableTilPositions.push_back(MyVector3(28, 87, 0));
	impassableTilPositions.push_back(MyVector3(28, 88, 0));
	impassableTilPositions.push_back(MyVector3(28, 89, 0));
	impassableTilPositions.push_back(MyVector3(28, 90, 0));
	impassableTilPositions.push_back(MyVector3(28, 91, 0));
	impassableTilPositions.push_back(MyVector3(28, 92, 0));
	impassableTilPositions.push_back(MyVector3(28, 93, 0));
	impassableTilPositions.push_back(MyVector3(28, 94, 0));
	impassableTilPositions.push_back(MyVector3(28, 95, 0));
	impassableTilPositions.push_back(MyVector3(28, 96, 0));
	impassableTilPositions.push_back(MyVector3(28, 97, 0));
	impassableTilPositions.push_back(MyVector3(28, 98, 0));
	impassableTilPositions.push_back(MyVector3(28, 99, 0));

	impassableTilPositions.push_back(MyVector3(35, 0, 0));
	impassableTilPositions.push_back(MyVector3(35, 1, 0));
	impassableTilPositions.push_back(MyVector3(35, 2, 0));
	impassableTilPositions.push_back(MyVector3(35, 3, 0));
	impassableTilPositions.push_back(MyVector3(35, 4, 0));
	impassableTilPositions.push_back(MyVector3(35, 5, 0));
	impassableTilPositions.push_back(MyVector3(35, 6, 0));
	impassableTilPositions.push_back(MyVector3(35, 7, 0));
	impassableTilPositions.push_back(MyVector3(35, 8, 0));
	impassableTilPositions.push_back(MyVector3(35, 9, 0));
	impassableTilPositions.push_back(MyVector3(35, 10, 0));
	impassableTilPositions.push_back(MyVector3(35, 11, 0));
	impassableTilPositions.push_back(MyVector3(35, 12, 0));
	//impassableTilPositions.push_back(MyVector3(35, 13, 0));
	//impassableTilPositions.push_back(MyVector3(35, 14, 0));
	//impassableTilPositions.push_back(MyVector3(35, 15, 0));
	//impassableTilPositions.push_back(MyVector3(35, 16, 0));
	//impassableTilPositions.push_back(MyVector3(35, 17, 0));
	//impassableTilPositions.push_back(MyVector3(35, 18, 0));
	//impassableTilPositions.push_back(MyVector3(35, 19, 0));
	//impassableTilPositions.push_back(MyVector3(35, 20, 0));
	impassableTilPositions.push_back(MyVector3(35, 21, 0));
	impassableTilPositions.push_back(MyVector3(35, 22, 0));
	impassableTilPositions.push_back(MyVector3(35, 23, 0));
	impassableTilPositions.push_back(MyVector3(35, 24, 0));
	impassableTilPositions.push_back(MyVector3(35, 25, 0));
	impassableTilPositions.push_back(MyVector3(35, 26, 0));
	impassableTilPositions.push_back(MyVector3(35, 27, 0));
	impassableTilPositions.push_back(MyVector3(35, 28, 0));
	impassableTilPositions.push_back(MyVector3(35, 29, 0));
	impassableTilPositions.push_back(MyVector3(35, 30, 0));
	impassableTilPositions.push_back(MyVector3(35, 31, 0));
	impassableTilPositions.push_back(MyVector3(35, 32, 0));
	impassableTilPositions.push_back(MyVector3(35, 33, 0));
	impassableTilPositions.push_back(MyVector3(35, 34, 0));
	impassableTilPositions.push_back(MyVector3(35, 35, 0));
	impassableTilPositions.push_back(MyVector3(35, 36, 0));
	impassableTilPositions.push_back(MyVector3(35, 37, 0));
	impassableTilPositions.push_back(MyVector3(35, 38, 0));
	impassableTilPositions.push_back(MyVector3(35, 39, 0));
	impassableTilPositions.push_back(MyVector3(35, 40, 0));
	impassableTilPositions.push_back(MyVector3(35, 41, 0));
	//impassableTilPositions.push_back(MyVector3(35, 42, 0));
	//impassableTilPositions.push_back(MyVector3(35, 43, 0));
	//impassableTilPositions.push_back(MyVector3(35, 44, 0));
	//impassableTilPositions.push_back(MyVector3(35, 45, 0));
	//impassableTilPositions.push_back(MyVector3(35, 46, 0));
	//impassableTilPositions.push_back(MyVector3(35, 47, 0));
	//impassableTilPositions.push_back(MyVector3(35, 48, 0));
	//impassableTilPositions.push_back(MyVector3(35, 49, 0));
	//impassableTilPositions.push_back(MyVector3(35, 50, 0));
	//impassableTilPositions.push_back(MyVector3(35, 51, 0));
	impassableTilPositions.push_back(MyVector3(35, 52, 0));
	impassableTilPositions.push_back(MyVector3(35, 53, 0));
	impassableTilPositions.push_back(MyVector3(35, 54, 0));
	impassableTilPositions.push_back(MyVector3(35, 55, 0));
	impassableTilPositions.push_back(MyVector3(35, 56, 0));
	impassableTilPositions.push_back(MyVector3(35, 57, 0));
	impassableTilPositions.push_back(MyVector3(35, 58, 0));
	impassableTilPositions.push_back(MyVector3(35, 59, 0));
	impassableTilPositions.push_back(MyVector3(35, 60, 0));
	impassableTilPositions.push_back(MyVector3(35, 61, 0));
	impassableTilPositions.push_back(MyVector3(35, 62, 0));
	impassableTilPositions.push_back(MyVector3(35, 63, 0));
	impassableTilPositions.push_back(MyVector3(35, 64, 0));
	impassableTilPositions.push_back(MyVector3(35, 65, 0));
	impassableTilPositions.push_back(MyVector3(35, 66, 0));
	impassableTilPositions.push_back(MyVector3(35, 67, 0));
	impassableTilPositions.push_back(MyVector3(35, 68, 0));
	impassableTilPositions.push_back(MyVector3(35, 69, 0));
	impassableTilPositions.push_back(MyVector3(35, 70, 0));
	impassableTilPositions.push_back(MyVector3(35, 71, 0));
	impassableTilPositions.push_back(MyVector3(35, 72, 0));
	impassableTilPositions.push_back(MyVector3(35, 73, 0));
	impassableTilPositions.push_back(MyVector3(35, 74, 0));
	impassableTilPositions.push_back(MyVector3(35, 75, 0));
	impassableTilPositions.push_back(MyVector3(35, 76, 0));
	impassableTilPositions.push_back(MyVector3(35, 77, 0));
	impassableTilPositions.push_back(MyVector3(35, 78, 0));
	impassableTilPositions.push_back(MyVector3(35, 79, 0));
	impassableTilPositions.push_back(MyVector3(35, 80, 0));
	impassableTilPositions.push_back(MyVector3(35, 81, 0));
	impassableTilPositions.push_back(MyVector3(35, 82, 0));
	impassableTilPositions.push_back(MyVector3(35, 83, 0));
	//impassableTilPositions.push_back(MyVector3(35, 84, 0));
	//impassableTilPositions.push_back(MyVector3(35, 85, 0));
	//impassableTilPositions.push_back(MyVector3(35, 86, 0));
	//impassableTilPositions.push_back(MyVector3(35, 87, 0));
	//impassableTilPositions.push_back(MyVector3(35, 88, 0));
	//impassableTilPositions.push_back(MyVector3(35, 89, 0));
	//impassableTilPositions.push_back(MyVector3(35, 90, 0));
	//impassableTilPositions.push_back(MyVector3(35, 91, 0));
	impassableTilPositions.push_back(MyVector3(35, 92, 0));
	impassableTilPositions.push_back(MyVector3(35, 93, 0));
	impassableTilPositions.push_back(MyVector3(35, 94, 0));
	impassableTilPositions.push_back(MyVector3(35, 95, 0));
	impassableTilPositions.push_back(MyVector3(35, 96, 0));
	impassableTilPositions.push_back(MyVector3(35, 97, 0));
	impassableTilPositions.push_back(MyVector3(35, 98, 0));
	impassableTilPositions.push_back(MyVector3(35, 99, 0));


	impassableTilPositions.push_back(MyVector3(42, 0, 0));
	impassableTilPositions.push_back(MyVector3(42, 1, 0));
	impassableTilPositions.push_back(MyVector3(42, 2, 0));
	impassableTilPositions.push_back(MyVector3(42, 3, 0));
	impassableTilPositions.push_back(MyVector3(42, 4, 0));
	impassableTilPositions.push_back(MyVector3(42, 5, 0));
	impassableTilPositions.push_back(MyVector3(42, 6, 0));
	impassableTilPositions.push_back(MyVector3(42, 7, 0));
	impassableTilPositions.push_back(MyVector3(42, 8, 0));
	impassableTilPositions.push_back(MyVector3(42, 9, 0));
	impassableTilPositions.push_back(MyVector3(42, 10, 0));
	//impassableTilPositions.push_back(MyVector3(42, 11, 0));
	//impassableTilPositions.push_back(MyVector3(42, 12, 0));
	//impassableTilPositions.push_back(MyVector3(42, 13, 0));
	//impassableTilPositions.push_back(MyVector3(42, 14, 0));
	impassableTilPositions.push_back(MyVector3(42, 15, 0));
	impassableTilPositions.push_back(MyVector3(42, 16, 0));
	impassableTilPositions.push_back(MyVector3(42, 17, 0));
	impassableTilPositions.push_back(MyVector3(42, 18, 0));
	impassableTilPositions.push_back(MyVector3(42, 19, 0));
	impassableTilPositions.push_back(MyVector3(42, 20, 0));
	impassableTilPositions.push_back(MyVector3(42, 21, 0));
	impassableTilPositions.push_back(MyVector3(42, 22, 0));
	impassableTilPositions.push_back(MyVector3(42, 23, 0));
	impassableTilPositions.push_back(MyVector3(42, 24, 0));
	impassableTilPositions.push_back(MyVector3(42, 25, 0));
	impassableTilPositions.push_back(MyVector3(42, 26, 0));
	impassableTilPositions.push_back(MyVector3(42, 27, 0));
	impassableTilPositions.push_back(MyVector3(42, 28, 0));
	impassableTilPositions.push_back(MyVector3(42, 29, 0));
	impassableTilPositions.push_back(MyVector3(42, 30, 0));
	impassableTilPositions.push_back(MyVector3(42, 31, 0));
	impassableTilPositions.push_back(MyVector3(42, 32, 0));
	impassableTilPositions.push_back(MyVector3(42, 33, 0));
	impassableTilPositions.push_back(MyVector3(42, 34, 0));
	//impassableTilPositions.push_back(MyVector3(42, 35, 0));
	//impassableTilPositions.push_back(MyVector3(42, 36, 0));
	//impassableTilPositions.push_back(MyVector3(42, 37, 0));
	//impassableTilPositions.push_back(MyVector3(42, 38, 0));
	//impassableTilPositions.push_back(MyVector3(42, 39, 0));
	//impassableTilPositions.push_back(MyVector3(42, 40, 0));
	//impassableTilPositions.push_back(MyVector3(42, 41, 0));
	//impassableTilPositions.push_back(MyVector3(42, 42, 0));
	//impassableTilPositions.push_back(MyVector3(42, 43, 0));
	//impassableTilPositions.push_back(MyVector3(42, 44, 0));
	impassableTilPositions.push_back(MyVector3(42, 45, 0));
	impassableTilPositions.push_back(MyVector3(42, 46, 0));
	impassableTilPositions.push_back(MyVector3(42, 47, 0));
	impassableTilPositions.push_back(MyVector3(42, 48, 0));
	impassableTilPositions.push_back(MyVector3(42, 49, 0));
	impassableTilPositions.push_back(MyVector3(42, 50, 0));
	impassableTilPositions.push_back(MyVector3(42, 51, 0));
	impassableTilPositions.push_back(MyVector3(42, 52, 0));
	impassableTilPositions.push_back(MyVector3(42, 53, 0));
	impassableTilPositions.push_back(MyVector3(42, 54, 0));
	impassableTilPositions.push_back(MyVector3(42, 55, 0));
	impassableTilPositions.push_back(MyVector3(42, 56, 0));
	impassableTilPositions.push_back(MyVector3(42, 57, 0));
	impassableTilPositions.push_back(MyVector3(42, 58, 0));
	impassableTilPositions.push_back(MyVector3(42, 59, 0));
	impassableTilPositions.push_back(MyVector3(42, 60, 0));
	impassableTilPositions.push_back(MyVector3(42, 61, 0));
	impassableTilPositions.push_back(MyVector3(42, 62, 0));
	impassableTilPositions.push_back(MyVector3(42, 63, 0));
	impassableTilPositions.push_back(MyVector3(42, 64, 0));
	impassableTilPositions.push_back(MyVector3(42, 65, 0));
	impassableTilPositions.push_back(MyVector3(42, 66, 0));
	impassableTilPositions.push_back(MyVector3(42, 67, 0));
	impassableTilPositions.push_back(MyVector3(42, 68, 0));
	impassableTilPositions.push_back(MyVector3(42, 69, 0));
	impassableTilPositions.push_back(MyVector3(42, 70, 0));
	impassableTilPositions.push_back(MyVector3(42, 71, 0));
	impassableTilPositions.push_back(MyVector3(42, 72, 0));
	impassableTilPositions.push_back(MyVector3(42, 73, 0));
	impassableTilPositions.push_back(MyVector3(42, 74, 0));
	impassableTilPositions.push_back(MyVector3(42, 75, 0));
	impassableTilPositions.push_back(MyVector3(42, 76, 0));
	impassableTilPositions.push_back(MyVector3(42, 77, 0));
	impassableTilPositions.push_back(MyVector3(42, 78, 0));
	impassableTilPositions.push_back(MyVector3(42, 79, 0));
	//impassableTilPositions.push_back(MyVector3(42, 80, 0));
	//impassableTilPositions.push_back(MyVector3(42, 81, 0));
	//impassableTilPositions.push_back(MyVector3(42, 82, 0));
	//impassableTilPositions.push_back(MyVector3(42, 83, 0));
	//impassableTilPositions.push_back(MyVector3(42, 84, 0));
	//impassableTilPositions.push_back(MyVector3(42, 85, 0));
	//impassableTilPositions.push_back(MyVector3(42, 86, 0));
	//impassableTilPositions.push_back(MyVector3(42, 87, 0));
	impassableTilPositions.push_back(MyVector3(42, 88, 0));
	impassableTilPositions.push_back(MyVector3(42, 89, 0));
	impassableTilPositions.push_back(MyVector3(42, 90, 0));
	impassableTilPositions.push_back(MyVector3(42, 91, 0));
	impassableTilPositions.push_back(MyVector3(42, 92, 0));
	impassableTilPositions.push_back(MyVector3(42, 93, 0));
	impassableTilPositions.push_back(MyVector3(42, 94, 0));
	impassableTilPositions.push_back(MyVector3(42, 95, 0));
	impassableTilPositions.push_back(MyVector3(42, 96, 0));
	impassableTilPositions.push_back(MyVector3(42, 97, 0));
	impassableTilPositions.push_back(MyVector3(42, 98, 0));
	impassableTilPositions.push_back(MyVector3(42, 99, 0));

	impassableTilPositions.push_back(MyVector3(49, 0, 0));
	impassableTilPositions.push_back(MyVector3(49, 1, 0));
	//impassableTilPositions.push_back(MyVector3(49, 2, 0));
	//impassableTilPositions.push_back(MyVector3(49, 3, 0));
	//impassableTilPositions.push_back(MyVector3(49, 4, 0));
	//impassableTilPositions.push_back(MyVector3(49, 5, 0));
	impassableTilPositions.push_back(MyVector3(49, 6, 0));
	impassableTilPositions.push_back(MyVector3(49, 7, 0));
	impassableTilPositions.push_back(MyVector3(49, 8, 0));
	impassableTilPositions.push_back(MyVector3(49, 9, 0));
	impassableTilPositions.push_back(MyVector3(49, 10, 0));
	impassableTilPositions.push_back(MyVector3(49, 11, 0));
	impassableTilPositions.push_back(MyVector3(49, 12, 0));
	impassableTilPositions.push_back(MyVector3(49, 13, 0));
	impassableTilPositions.push_back(MyVector3(49, 14, 0));
	impassableTilPositions.push_back(MyVector3(49, 15, 0));
	impassableTilPositions.push_back(MyVector3(49, 16, 0));
	impassableTilPositions.push_back(MyVector3(49, 17, 0));
	impassableTilPositions.push_back(MyVector3(49, 18, 0));
	impassableTilPositions.push_back(MyVector3(49, 19, 0));
	impassableTilPositions.push_back(MyVector3(49, 20, 0));
	impassableTilPositions.push_back(MyVector3(49, 21, 0));
	impassableTilPositions.push_back(MyVector3(49, 22, 0));
	impassableTilPositions.push_back(MyVector3(49, 23, 0));
	impassableTilPositions.push_back(MyVector3(49, 24, 0));
	impassableTilPositions.push_back(MyVector3(49, 25, 0));
	impassableTilPositions.push_back(MyVector3(49, 26, 0));
	impassableTilPositions.push_back(MyVector3(49, 27, 0));
	impassableTilPositions.push_back(MyVector3(49, 28, 0));
	impassableTilPositions.push_back(MyVector3(49, 29, 0));
	impassableTilPositions.push_back(MyVector3(49, 30, 0));
	impassableTilPositions.push_back(MyVector3(49, 31, 0));
	impassableTilPositions.push_back(MyVector3(49, 32, 0));
	impassableTilPositions.push_back(MyVector3(49, 33, 0));
	impassableTilPositions.push_back(MyVector3(49, 34, 0));
	//impassableTilPositions.push_back(MyVector3(49, 35, 0));
	//impassableTilPositions.push_back(MyVector3(49, 36, 0));
	//impassableTilPositions.push_back(MyVector3(49, 37, 0));
	impassableTilPositions.push_back(MyVector3(49, 38, 0));
	impassableTilPositions.push_back(MyVector3(49, 39, 0));
	impassableTilPositions.push_back(MyVector3(49, 40, 0));
	impassableTilPositions.push_back(MyVector3(49, 41, 0));
	impassableTilPositions.push_back(MyVector3(49, 42, 0));
	impassableTilPositions.push_back(MyVector3(49, 43, 0));
	impassableTilPositions.push_back(MyVector3(49, 44, 0));
	impassableTilPositions.push_back(MyVector3(49, 45, 0));
	impassableTilPositions.push_back(MyVector3(49, 46, 0));
	impassableTilPositions.push_back(MyVector3(49, 47, 0));
	impassableTilPositions.push_back(MyVector3(49, 48, 0));
	impassableTilPositions.push_back(MyVector3(49, 49, 0));
	impassableTilPositions.push_back(MyVector3(49, 50, 0));
	impassableTilPositions.push_back(MyVector3(49, 51, 0));
	impassableTilPositions.push_back(MyVector3(49, 52, 0));
	impassableTilPositions.push_back(MyVector3(49, 53, 0));
	impassableTilPositions.push_back(MyVector3(49, 54, 0));
	impassableTilPositions.push_back(MyVector3(49, 55, 0));
	impassableTilPositions.push_back(MyVector3(49, 56, 0));
	impassableTilPositions.push_back(MyVector3(49, 57, 0));
	impassableTilPositions.push_back(MyVector3(49, 58, 0));
	impassableTilPositions.push_back(MyVector3(49, 59, 0));
	impassableTilPositions.push_back(MyVector3(49, 60, 0));
	impassableTilPositions.push_back(MyVector3(49, 61, 0));
	impassableTilPositions.push_back(MyVector3(49, 62, 0));
	impassableTilPositions.push_back(MyVector3(49, 63, 0));
	impassableTilPositions.push_back(MyVector3(49, 64, 0));
	impassableTilPositions.push_back(MyVector3(49, 65, 0));
	//impassableTilPositions.push_back(MyVector3(49, 66, 0));
	//impassableTilPositions.push_back(MyVector3(49, 67, 0));
	//impassableTilPositions.push_back(MyVector3(49, 68, 0));
	//impassableTilPositions.push_back(MyVector3(49, 69, 0));
	//impassableTilPositions.push_back(MyVector3(49, 70, 0));
	//impassableTilPositions.push_back(MyVector3(49, 71, 0));
	impassableTilPositions.push_back(MyVector3(49, 72, 0));
	impassableTilPositions.push_back(MyVector3(49, 73, 0));
	impassableTilPositions.push_back(MyVector3(49, 74, 0));
	impassableTilPositions.push_back(MyVector3(49, 75, 0));
	impassableTilPositions.push_back(MyVector3(49, 76, 0));
	impassableTilPositions.push_back(MyVector3(49, 77, 0));
	impassableTilPositions.push_back(MyVector3(49, 78, 0));
	impassableTilPositions.push_back(MyVector3(49, 79, 0));
	impassableTilPositions.push_back(MyVector3(49, 80, 0));
	impassableTilPositions.push_back(MyVector3(49, 81, 0));
	impassableTilPositions.push_back(MyVector3(49, 82, 0));
	impassableTilPositions.push_back(MyVector3(49, 83, 0));
	//impassableTilPositions.push_back(MyVector3(49, 84, 0));
	//impassableTilPositions.push_back(MyVector3(49, 85, 0));
	//impassableTilPositions.push_back(MyVector3(49, 86, 0));
	impassableTilPositions.push_back(MyVector3(49, 87, 0));
	impassableTilPositions.push_back(MyVector3(49, 88, 0));
	impassableTilPositions.push_back(MyVector3(49, 89, 0));
	impassableTilPositions.push_back(MyVector3(49, 90, 0));
	impassableTilPositions.push_back(MyVector3(49, 91, 0));
	impassableTilPositions.push_back(MyVector3(49, 92, 0));
	impassableTilPositions.push_back(MyVector3(49, 93, 0));
	impassableTilPositions.push_back(MyVector3(49, 94, 0));
	//impassableTilPositions.push_back(MyVector3(49, 95, 0));
	//impassableTilPositions.push_back(MyVector3(49, 96, 0));
	//impassableTilPositions.push_back(MyVector3(49, 97, 0));
	//impassableTilPositions.push_back(MyVector3(49, 98, 0));
	//impassableTilPositions.push_back(MyVector3(49, 99, 0));

	impassableTilPositions.push_back(MyVector3(70, 0, 0));
	impassableTilPositions.push_back(MyVector3(70, 1, 0));
	impassableTilPositions.push_back(MyVector3(70, 2, 0));
	impassableTilPositions.push_back(MyVector3(70, 3, 0));
	impassableTilPositions.push_back(MyVector3(70, 4, 0));
	impassableTilPositions.push_back(MyVector3(70, 5, 0));
	impassableTilPositions.push_back(MyVector3(70, 6, 0));
	impassableTilPositions.push_back(MyVector3(70, 7, 0));
	impassableTilPositions.push_back(MyVector3(70, 8, 0));
	impassableTilPositions.push_back(MyVector3(70, 9, 0));
	//impassableTilPositions.push_back(MyVector3(70, 10, 0));
	//impassableTilPositions.push_back(MyVector3(70, 11, 0));
	//impassableTilPositions.push_back(MyVector3(70, 12, 0));
	//impassableTilPositions.push_back(MyVector3(70, 13, 0));
	//impassableTilPositions.push_back(MyVector3(70, 14, 0));
	//impassableTilPositions.push_back(MyVector3(70, 15, 0));
	//impassableTilPositions.push_back(MyVector3(70, 16, 0));
	//impassableTilPositions.push_back(MyVector3(70, 17, 0));
	impassableTilPositions.push_back(MyVector3(70, 18, 0));
	impassableTilPositions.push_back(MyVector3(70, 19, 0));
	impassableTilPositions.push_back(MyVector3(70, 20, 0));
	impassableTilPositions.push_back(MyVector3(70, 21, 0));
	impassableTilPositions.push_back(MyVector3(70, 22, 0));
	impassableTilPositions.push_back(MyVector3(70, 23, 0));
	impassableTilPositions.push_back(MyVector3(70, 24, 0));
	impassableTilPositions.push_back(MyVector3(70, 25, 0));
	impassableTilPositions.push_back(MyVector3(70, 26, 0));
	impassableTilPositions.push_back(MyVector3(70, 27, 0));
	//impassableTilPositions.push_back(MyVector3(70, 28, 0));
	//impassableTilPositions.push_back(MyVector3(70, 29, 0));
	impassableTilPositions.push_back(MyVector3(70, 30, 0));
	impassableTilPositions.push_back(MyVector3(70, 31, 0));
	impassableTilPositions.push_back(MyVector3(70, 32, 0));
	impassableTilPositions.push_back(MyVector3(70, 33, 0));
	impassableTilPositions.push_back(MyVector3(70, 34, 0));
	impassableTilPositions.push_back(MyVector3(70, 35, 0));
	impassableTilPositions.push_back(MyVector3(70, 36, 0));
	//impassableTilPositions.push_back(MyVector3(70, 37, 0));
	//impassableTilPositions.push_back(MyVector3(70, 38, 0));
	//impassableTilPositions.push_back(MyVector3(70, 39, 0));
	//impassableTilPositions.push_back(MyVector3(70, 40, 0));
	//impassableTilPositions.push_back(MyVector3(70, 41, 0));
	impassableTilPositions.push_back(MyVector3(70, 42, 0));
	impassableTilPositions.push_back(MyVector3(70, 43, 0));
	impassableTilPositions.push_back(MyVector3(70, 44, 0));
	impassableTilPositions.push_back(MyVector3(70, 45, 0));
	impassableTilPositions.push_back(MyVector3(70, 46, 0));
	impassableTilPositions.push_back(MyVector3(70, 47, 0));
	impassableTilPositions.push_back(MyVector3(70, 48, 0));
	impassableTilPositions.push_back(MyVector3(70, 49, 0));
	impassableTilPositions.push_back(MyVector3(70, 50, 0));
	impassableTilPositions.push_back(MyVector3(70, 51, 0));
	impassableTilPositions.push_back(MyVector3(70, 52, 0));
	impassableTilPositions.push_back(MyVector3(70, 53, 0));
	impassableTilPositions.push_back(MyVector3(70, 54, 0));
	impassableTilPositions.push_back(MyVector3(70, 55, 0));
	impassableTilPositions.push_back(MyVector3(70, 56, 0));
	impassableTilPositions.push_back(MyVector3(70, 57, 0));
	impassableTilPositions.push_back(MyVector3(70, 58, 0));
	impassableTilPositions.push_back(MyVector3(70, 59, 0));
	impassableTilPositions.push_back(MyVector3(70, 60, 0));
	impassableTilPositions.push_back(MyVector3(70, 61, 0));
	impassableTilPositions.push_back(MyVector3(70, 62, 0));
	impassableTilPositions.push_back(MyVector3(70, 63, 0));
	impassableTilPositions.push_back(MyVector3(70, 64, 0));
	impassableTilPositions.push_back(MyVector3(70, 65, 0));
	impassableTilPositions.push_back(MyVector3(70, 66, 0));
	//impassableTilPositions.push_back(MyVector3(70, 67, 0));
	//impassableTilPositions.push_back(MyVector3(70, 68, 0));
	//impassableTilPositions.push_back(MyVector3(70, 69, 0));
	//impassableTilPositions.push_back(MyVector3(70, 70, 0));
	//impassableTilPositions.push_back(MyVector3(70, 71, 0));
	//impassableTilPositions.push_back(MyVector3(70, 72, 0));
	//impassableTilPositions.push_back(MyVector3(70, 73, 0));
	impassableTilPositions.push_back(MyVector3(70, 74, 0));
	impassableTilPositions.push_back(MyVector3(70, 75, 0));
	impassableTilPositions.push_back(MyVector3(70, 76, 0));
	impassableTilPositions.push_back(MyVector3(70, 77, 0));
	impassableTilPositions.push_back(MyVector3(70, 78, 0));
	impassableTilPositions.push_back(MyVector3(70, 79, 0));
	impassableTilPositions.push_back(MyVector3(70, 80, 0));
	impassableTilPositions.push_back(MyVector3(70, 81, 0));
	impassableTilPositions.push_back(MyVector3(70, 82, 0));
	impassableTilPositions.push_back(MyVector3(70, 83, 0));
	impassableTilPositions.push_back(MyVector3(70, 84, 0));
	impassableTilPositions.push_back(MyVector3(70, 85, 0));
	impassableTilPositions.push_back(MyVector3(70, 86, 0));
	impassableTilPositions.push_back(MyVector3(70, 87, 0));
	impassableTilPositions.push_back(MyVector3(70, 88, 0));
	impassableTilPositions.push_back(MyVector3(70, 89, 0));
	impassableTilPositions.push_back(MyVector3(70, 90, 0));
	//impassableTilPositions.push_back(MyVector3(70, 91, 0));
	//impassableTilPositions.push_back(MyVector3(70, 92, 0));
	//impassableTilPositions.push_back(MyVector3(70, 93, 0));
	//impassableTilPositions.push_back(MyVector3(70, 94, 0));
	//impassableTilPositions.push_back(MyVector3(70, 95, 0));
	//impassableTilPositions.push_back(MyVector3(70, 96, 0));
	impassableTilPositions.push_back(MyVector3(70, 97, 0));
	impassableTilPositions.push_back(MyVector3(70, 98, 0));
	impassableTilPositions.push_back(MyVector3(70, 99, 0));
}
