#include "NPC.h"

NPC::NPC(int t_ID, sf::Vector2f t_pos, float size, Graph<NodeData, int>* t_graph) : m_graph(t_graph)
{
	ID = t_ID;
	m_myTile = new Tile(t_pos, size);
	m_myTile->setColourBlue();

	m_clock.restart(); //restart the clock
}

NPC::~NPC()
{

}

void NPC::render(sf::RenderWindow* t_rendWind)
{
	m_myTile->render(t_rendWind);
}

void NPC::update(sf::Time t_deltaTime)
{
	if (m_path.size() > 0)
	{
		if (m_clock.getElapsedTime() > t_deltaTime)
		{
			walkPath();
			m_clock.restart();
		}
	}
}

void NPC::passGraph(Graph<NodeData, int>* t_graph)
{
	m_graph = t_graph;
}

void NPC::unMarkGraph()
{
	for (int i = 0; i < m_graph->getNodes()->size(); i++)
	{
		m_graph->getNodes()->at(i)->setMarked(false, ID);
	}
}

//auto lambda_GetPathToGoal = [](NPC* m_npcObj)
//{
//	Node* startNode = m_npcObj->getNodeFromPosition(m_npcObj->m_myTile->getPosition());
//	Node* goalNode = m_npcObj->getNodeFromPosition(m_npcObj->m_goalPosition);
//	m_npcObj->unMarkGraph();  // unmark all nodes so fresh graph
//	m_npcObj->m_graph->aStar(m_npcObj->ID, startNode, goalNode, visit, m_npcObj->m_path);
//};

void NPC::getPathToGoal()
{
	//std::cout << "SUCCESS" << std::endl;
	//Node* startNode = getNodeFromPosition(m_myTile->getPosition());
	//Node* goalNode = getNodeFromPosition(m_goalPosition);
	//unMarkGraph();  // unmark all nodes so fresh graph
	//m_graph->aStar(startNode, goalNode, visit, m_path);
}

Node* NPC::getNodeFromPosition(sf::Vector2f t_pos)
{
	int index = 0;
	
	//std::cout << m_graph->getNodes()->at(index)->m_data.m_position.x << ", " << m_graph->getNodes()->at(index)->m_data.m_position.y << std::endl;
	while (m_graph->getNodes()->at(index)->m_data.m_position != (MyVector3)t_pos)
	{
		if (index < m_graph->getNodes()->size())
		{
			index++;
		}
		else {
			continue;
		}
	}
	return m_graph->getNodes()->at(index);
}

void NPC::walkPath()
{
	if (m_path.size() > 0 && ((MyVector3)m_myTile->getPosition() - m_path.front()->m_data.m_position).length() == 0)
	{
		//if (path.size() == 1)
		//{
		//	path.clear(); // because the erase below adds 1
		//}
		m_path.erase(m_path.begin());
	}
	//unMarkGraph();  // unmark all nodes so fresh graph
	
	if (m_path.size() > 0)
	{
		//if ((m_enemyOne.m_enemyPos - path.front()->m_data.m_position).length() <= 0.5)
		//{
		//	//m_enemyOne.m_enemyPos = path.front()->m_data.m_position; // set enemy pos to node pos before deleting
		//	//path.erase(path.begin(), path.begin() + 1);
		//}

		m_myTile->setPosition(m_path.front()->m_data.m_position);
		m_path.erase(m_path.begin(), m_path.begin() + 1);
	}
}

void NPC::createThread(NPC* m_npc)
{
	//threadForPathfinding = std::thread(lambda_GetPathToGoal, m_npc);
	//threadID_Vec.at(ID) = threadForPathfinding.get_id();  // set the id for the id at this NPC_ID position in the vec (used in compare predicate)
	//threadForPathfinding.join();  //remove
}

void NPC::setGoalPosition(sf::Vector2f t_goalPosition)
{
	m_goalPosition = t_goalPosition;
}
