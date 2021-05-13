
// The NPC object

#include "SFML/Graphics.hpp"
#include "Tile.h"
#include "Graph.h"
#include "NodeData.h"


typedef GraphArc<NodeData, int> Arc;
typedef GraphNode<NodeData, int> Node;
class NPC {

public:
	NPC(sf::Vector2f t_pos, float size);
	~NPC();
	void render(sf::RenderWindow* t_rendWind);
	void update(sf::Time t_deltaTime);
	void passGraph(Graph<NodeData, int>* t_graph);
	void getPathToGoal(sf::Vector2f t_goal);
	Node* getNodeFromPosition(sf::Vector2f t_pos);

	void walkPath();

private:
	Tile* m_myTile; // The tile representing this NPC
	Graph<NodeData, int>* m_graph; // pointer to the graph
	std::vector<Node*> m_path; // The path we get from A star call
};