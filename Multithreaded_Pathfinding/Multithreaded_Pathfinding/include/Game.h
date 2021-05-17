#pragma once

// Made by: Josh Browne

#include <iostream>
#include "SFML/Graphics.hpp"
#include "Tile.h"
#include "Globals.h"
#include "Graph.h"
#include "NodeData.h"
#include "NPC.h"
#include "ThreadPool.h"
#include "MyVector3.h"

typedef GraphArc<NodeData, int> Arc;
typedef GraphNode<NodeData, int> Node;
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

	void initializeGraph();
	void initImpassableTiles();
	void setGraphNeighbours();

	void GetPathToGoal(NPC* m_npcObj);

	bool isPosOnImpassable(MyVector3 t_pos);

private:
	sf::RenderWindow m_renderWin; // main SFML window

	const int mapSize = 50; // the number of nodes wide and high we want the map
	std::vector<Tile> m_TileMap; // the tile map (vector of tile objects)
	std::vector<MyVector3> impassableTilPositions;
	float m_tileSize;
	Graph<NodeData, int>* graph;

	std::vector<NPC*> m_npcContainer;
	int goalX;
	int	goalY;
	sf::Vector2f goalPosition;
	ThreadPool m_thread_pool;
};

