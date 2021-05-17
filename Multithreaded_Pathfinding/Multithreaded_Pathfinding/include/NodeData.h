#pragma once
#include <string>
#include <queue> //
#include "Graph.h" //
#include <SFML\Graphics.hpp>
#include "MyVector3.h"
#include "Globals.h"

template <class NodeType, class ArcType> class GraphArc;
template <class NodeType, class ArcType> class GraphNode;
// ---------------------------------------------------------------------
//  Name:           NodeData
//  Description:    A simple struct to encapsulate all the node data.
//                  Add or remove fields as required.               
// ---------------------------------------------------------------------
struct NodeData
{
	MyVector3 m_position;
	std::string m_name;
	bool m_isPassable{ true };
	std::vector<float> m_pathCost = std::vector<float>(m_npcCount, 0.f); // g(n)
	std::vector<float> m_distanceToGoal = std::vector<float>(m_npcCount, 0.f); // h(n)
	std::vector<float> m_totalEstimatedDistance = std::vector<float>(m_npcCount, 0.f); // f(n)

	sf::CircleShape m_circle;
};


// comparer for A*
template<class NodeType, class ArcType>
class NodeSearchCostComparerAstar
{
public:
	typedef GraphArc<NodeType, ArcType> Arc;
	typedef GraphNode<NodeType, ArcType> Node;

	//NodeSearchCostComparerAstar(int t_npcID) : m_npcID(t_npcID) {};

	int m_npcID;
	bool operator()(Node* n1, Node* n2)
	{
		for (int i = 0; i < m_npcCount; i++)
		{
			if (threadID_Vec.at(i) == std::this_thread::get_id())
			{
				m_npcID = i;
			}
		}
		return n1->m_data.m_totalEstimatedDistance.at(m_npcID) > n2->m_data.m_totalEstimatedDistance.at(m_npcID);
	}
};


template <typename Data, typename Container, typename Predicate>
class MyPriorityQueue : public std::priority_queue<Data, Container, Predicate>
{
public:
	//MyPriorityQueue(NodeSearchCostComparerAstar<NodeType, ArcType>* aStarComp) {
	//	this->comp = aStarComp;
	//}
	// std::priority_queue has two useful members:
	// 1. this->c is the underlying container of a priority_queue
	// 2. this->comp is the comparison predicate
	void reorder()
	{

		// std::make_heap rearranges the elements in the range [first,last] in such 
		//  a way that they form a heap.
		// std::begin() returns an iterator to the beginning of the given container c 		
		std::make_heap(std::begin(this->c), std::end(this->c), this->comp);
	}
};
