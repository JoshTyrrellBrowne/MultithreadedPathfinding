
// The NPC object
#include <iostream>
#include "SFML/Graphics.hpp"
#include "Tile.h"
#include "Graph.h"
#include "NodeData.h"
#include <thread>
#include "Globals.h"


typedef GraphArc<NodeData, int> Arc;
typedef GraphNode<NodeData, int> Node;
class NPC {

public:
	NPC(int t_ID, sf::Vector2f t_pos, float size, Graph<NodeData, int>* t_graph);
	~NPC();
	void render(sf::RenderWindow* t_rendWind);
	void update(sf::Time t_deltaTime);
	void passGraph(Graph<NodeData, int>* t_graph);
	void unMarkGraph();
	void getPathToGoal();
	Node* getNodeFromPosition(sf::Vector2f t_pos);
	void walkPath();
	void createThread(NPC* m_npc);
	void setGoalPosition(sf::Vector2f t_goalPosition);
	
	int ID; // the ID for this NPC
	Tile* m_myTile; // The tile representing this NPC
	Graph<NodeData, int>* m_graph; // pointer to the graph
	std::vector<Node*> m_path; // The path we get from A star call
	sf::Vector2f m_goalPosition;
private:
	sf::Clock m_clock; // clock used for slowing pathfinding

	//std::thread threadForPathfinding;  // this points to our pathfinding thread
};