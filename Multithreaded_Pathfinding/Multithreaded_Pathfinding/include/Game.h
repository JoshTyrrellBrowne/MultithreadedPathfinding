#pragma once

// Made by: Josh Browne

#include <iostream>
#include "SFML/Graphics.hpp"
#include "Tile.h"

#include "Graph.h"
#include "NodeData.h"
#include "NPC.h"

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
	void setGraphNeighbours();

private:
	sf::RenderWindow m_renderWin; // main SFML window

	std::vector<Tile> m_TileMap; // the tile map (vector of tile objects)
	float m_tileSize;
	Graph<NodeData, int>* graph;

	NPC* m_npc;
};

