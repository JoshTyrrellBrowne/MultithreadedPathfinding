#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <queue>
#include <vector>
#include <functional>
#include "NodeData.h"
#include "SFML/Graphics.hpp"


template <class NodeType, class ArcType> class GraphArc;
template <class NodeType, class ArcType> class GraphNode;

// ---------------------------------------------------------------------
//  Name:           Graph
//  Description:    Manages the nodes and connections (arc) between them
// ---------------------------------------------------------------------
template<class NodeType, class ArcType>
class Graph 
{
public:        
	// typedef the classes to make our lives easier.
	typedef GraphArc<NodeType, ArcType> Arc;
	typedef GraphNode<NodeType, ArcType> Node;

    // Constructor and destructor functions
    Graph( int size );
    ~Graph();

    // Accessors
    Node * nodeIndex(int index) const 
	{
		return m_nodes.at(index);
    }

    // Public member functions.
    bool addNode( NodeType data, int index );
    void removeNode( int index );
    bool addArc( int from, int to, ArcType weight );
    void removeArc( int from, int to );
    Arc* getArc( int from, int to );        
    void clearMarks();
   
	void aStar(int npcID, Node* start, Node* dest, std::function<void(Node*)> f_visit, std::vector<Node*>& path);

	std::vector<Node*>* getNodes() { return &m_nodes; };

	int getIndexFromName(std::string);
	int getIndexFromNode(Node t_node);
	bool getNodeFromPosition(sf::Vector2f t_pos, Node* t_nodePtr);
private:
	

	// ----------------------------------------------------------------
	//  Description:    A container of all the nodes in the graph.
	// ----------------------------------------------------------------

	std::vector<Node*> m_nodes;
};

// ----------------------------------------------------------------
//  Name:           Graph
//  Description:    Constructor, this constructs an empty graph
//  Arguments:      The maximum number of nodes.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::Graph( int maxNodes ) : m_nodes( maxNodes, nullptr) 
{  
}

// ----------------------------------------------------------------
//  Name:           ~Graph
//  Description:    destructor, This deletes every node
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::~Graph() {
   for( int index = 0; index < m_nodes.size(); index++ ) 
   {
        if( m_nodes[index] != nullptr ) 
		{
			delete m_nodes.at(index);
        }
   }
}

// ----------------------------------------------------------------
//  Name:           addNode
//  Description:    This adds a node at a given index in the graph.
//  Arguments:      The first parameter is the data to store in the node.
//                  The second parameter is the index to store the node.
//  Return Value:   true if successful
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addNode( NodeType data, int index ) 
{
   bool nodeNotPresent = false;
   // find out if a node does not exist at that index.
   if ( nullptr == m_nodes.at(index) ) 
   {
      nodeNotPresent = true;
      // create a new node, put the data in it, and unmark it.
	  m_nodes.at(index) = new Node;
	  m_nodes.at(index)->m_data = data;
	  //m_nodes.at(index)->setMarked(false);
    }
        
    return nodeNotPresent;
}

// ----------------------------------------------------------------
//  Name:           removeNode
//  Description:    This removes a node from the graph
//  Arguments:      The index of the node to return.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeNode( int index ) 
{
     // Only proceed if node does exist.
     if( nullptr != m_nodes.at(index) ) 
	 {
         // now find every arc that points to the node that
         // is being removed and remove it.        
         Arc* arc;

         // loop through every node
         for( int node = 0; node < m_nodes.size(); node++ ) 
		 {
              // if the node is valid...
              if( nullptr != m_nodes.at(node) ) {
                  // see if the node has an arc pointing to the current node.
                  arc = m_nodes.at(node)->getArc(m_nodes.at(index) );
              }
              // if it has an arc pointing to the current node, then
              // remove the arc.
              if( arc != 0 ) {
                  removeArc( node, index );
              }
         }
        

        // now that every arc pointing to the current node has been removed,
        // the node can be deleted.
        delete m_nodes.at(index);
		m_nodes.at(index) = nullptr;       
    }
}

// ----------------------------------------------------------------
//  Name:           addArd
//  Description:    Adds an arc from the first index to the 
//                  second index with the specified weight.
//  Arguments:      The first argument is the originating node index
//                  The second argument is the ending node index
//                  The third argument is the weight of the arc
//  Return Value:   true on success.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addArc( int from, int to, ArcType weight ) {
     bool proceed = true; 
     // make sure both nodes exist.
     if( nullptr == m_nodes.at(from) || nullptr == m_nodes.at(to) ) 
	 {
         proceed = false;
     }
        
     // if an arc already exists we should not proceed
     if( m_nodes.at(from)->getArc( m_nodes.at(to) ) != nullptr ) 
	 {
         proceed = false;
     }

     if (proceed == true) 
	 {
        // add the arc to the "from" node.
		 m_nodes.at(from)->addArc(m_nodes.at(to), weight );
     }
        
     return proceed;
}

// ----------------------------------------------------------------
//  Name:           removeArc
//  Description:    This removes the arc from the first index to the second index
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeArc( int from, int to ) 
{
     // Make sure that the node exists before trying to remove
     // an arc from it.
     bool nodeExists = true;
     if( nullptr == m_nodes.at(from) || nullptr == m_nodes.at(to) ) 
	 {
         nodeExists = false;
     }

     if (nodeExists == true) 
	 {
        // remove the arc.
		 m_nodes.at(from)->removeArc(m_nodes.at(to) );
     }
}


// ----------------------------------------------------------------
//  Name:           getArc
//  Description:    Gets a pointer to an arc from the first index
//                  to the second index.
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   pointer to the arc, or 0 if it doesn't exist.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
GraphArc<NodeType, ArcType>* Graph<NodeType, ArcType>::getArc( int from, int to ) 
{
     Arc* arc = 0;
     // make sure the to and from nodes exist
     if( nullptr != m_nodes.at(from) && nullptr != m_nodes.at(to) ) 
	 {
         arc = m_nodes.at(from)->getArc(m_nodes.at(to) );
     }
                
     return arc;
}


// ----------------------------------------------------------------
//  Name:           clearMarks
//  Description:    This clears every mark on every node.
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::clearMarks() 
{
     for( int index = 0; index < m_nodes.size(); index++ ) 
	 {
          if( nullptr != m_nodes.at(index) ) 
		  {
			  m_nodes.at(index)->setMarked(false);
          }
     }
}


// IMPORTANT: this A-star is modified to behave in a way where the node actors pathfind to the goal with ambush style paths
template<class NodeType, class ArcType>
inline void Graph<NodeType, ArcType>::aStar(int npcID, Node* start, Node* dest, std::function<void(Node*)> f_visit, std::vector<Node*>& path)
{
	MyPriorityQueue<Node*, std::vector<Node*>, NodeSearchCostComparerAstar<NodeType, ArcType>> pq;
	
	for (Node* node : this->m_nodes)
	{
		node->setPrevious(npcID, nullptr);  // JOSH MIGHT NEED CHANGE THIS
		//node->m_data.m_pathCost = std::numeric_limits<int>::max(); // large number (substitute for infinite)
		node->m_data.m_totalEstimatedDistance.at(npcID) = std::numeric_limits<int>::max(); // large number (substitute for infinite)
		node->m_data.m_distanceToGoal.at(npcID) = (node->m_data.m_position - dest->m_data.m_position).length(); //euclidian distance

		//node->m_data.m_distanceToGoal.at(npcID) = sqrt(
		//	(node->m_data.m_position.x - dest->m_data.m_position.x) * (node->m_data.m_position.x - dest->m_data.m_position.x) +       // manhattan distance
		//	(node->m_data.m_position.y - dest->m_data.m_position.y) * (node->m_data.m_position.y - dest->m_data.m_position.y)
		//);
		//node->m_data.m_totalEstimatedDistance = node->m_data.m_distanceToGoal; // 
	}

	start->m_data.m_pathCost.at(npcID) = 0;
	start->setPrevious(npcID, nullptr);
	start->m_data.m_totalEstimatedDistance.at(npcID) = start->m_data.m_distanceToGoal.at(npcID);
	pq.push(start);
	for (int i = 0; i < m_npcCount; i++)
	{
		start->setMarked(true, i);
	}
	float extraWeight = 0;  // the extra weight to add for A*mbush (to increase variety of paths)
	while (pq.size() != 0 && pq.top() != dest)
	{
		auto iter = pq.top()->arcList().begin();  // arc to neighbour node
		auto endIter = pq.top()->arcList().end();

		for (; iter != endIter; iter++) // loop through the kids of pq.top()
		{
			if ((*iter).node() != pq.top()->getPrevious(npcID)) // dont go backwards
			{
				if ((*iter).node()->m_data.m_isPassable == false) // if not passable, dont do nothin
				{
					continue;
				}

				extraWeight = 0; // reset, clear weights from previous ark calc
				// A*mbush, check if any other npc's have this node in their path
				for (int i = 0; i < m_npcCount; i++)
				{
					if (i != npcID)  // dont need to check our own previous
					{
						if ((*iter).node()->getPrevious(i) != nullptr)  // another npc has processed this node
						{
							//(*iter).m_weight += (*iter).m_weight;
							//(*iter).increaseWeight();

							extraWeight += ((*iter).weight() * 7);
						}
					}	
				}
	
				// this nodes distance to goal + path cost so far + this arc's weight
				float distChild = (*iter).node()->m_data.m_distanceToGoal.at(npcID) + (pq.top()->m_data.m_pathCost.at(npcID) + (*iter).weight())
					+ extraWeight; // add extra weight, A*mbush

				//(*iter).node()->m_data.m_totalEstimatedDistance.at(npcID) = (*iter).node()->m_data.m_distanceToGoal.at(npcID) + (pq.top()->m_data.m_pathCost.at(npcID) + (*iter).weight());

				if (distChild < (*iter).node()->m_data.m_totalEstimatedDistance.at(npcID))
				//if (distChild < (*iter).node()->m_data.m_distanceToGoal.at(npcID) + )
				{
					f_visit((*iter).node());
					(*iter).node()->m_data.m_pathCost.at(npcID) = (*iter).weight() + pq.top()->m_data.m_pathCost.at(npcID);
					(*iter).node()->m_data.m_totalEstimatedDistance.at(npcID) = distChild;
					(*iter).node()->setPrevious(npcID, pq.top());
				}
				if (!((*iter).node()->marked(npcID))) // if not marked
				{
					pq.push((*iter).node());
					(*iter).node()->setMarked(true, npcID);
				}
			}
		}
		pq.pop();
	}
	Node* current = dest;
	while (current->getPrevious(npcID) != nullptr)
	{
		path.push_back(current);
		current = current->getPrevious(npcID);
	}
	path.push_back(current); // just to catch the last one 

	std::reverse(path.begin(), path.end());
}


template<class NodeType, class ArcType>
inline int Graph<NodeType, ArcType>::getIndexFromName(std::string t_name)
{
	auto it = m_nodes.begin();
	int index = 0;
	while (m_nodes.at(index)->m_data.m_name != t_name)
	{
		index++;
		if (index > m_nodes.size() -1)
		{
			index = -1;
			break;
		}
	}
	return index;
}

template<class NodeType, class ArcType>
inline int Graph<NodeType, ArcType>::getIndexFromNode(Node t_node)
{
	int index = 0;
	while (m_nodes.at(index)->m_data.m_name != t_node->m_data.m_name)
	{
		index++;
		if (index > m_nodes.size() - 1)
		{
			index = -1;
			break;
		}
	}

	return index;
}

template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::getNodeFromPosition(sf::Vector2f t_pos, Node* t_nodePtr)
{
	int index = 0;
	//std::cout << m_nodes.at(index)->m_data.m_position.x << ", " << m_nodes.at(index)->m_data.m_position.y << std::endl;
	while (m_nodes.at(index)->m_data.m_position != (MyVector3)t_pos)
	{
		index++;
		if (index > m_nodes.size())
		{
			return false;
		}
	}
	*t_nodePtr = *m_nodes.at(index);
	return true;
}


#include "GraphNode.h"
#include "GraphArc.h"


#endif