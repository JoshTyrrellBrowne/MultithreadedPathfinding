#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include "Globals.h"
#include <list>


// Forward references
template <typename NodeType, typename ArcType> class GraphArc;

// -------------------------------------------------------
// Name:        GraphNode
// Description: This is the node class. The node class 
//              contains data, and has a linked list of 
//              arcs.
// -------------------------------------------------------
template<class NodeType, class ArcType>
class GraphNode 
{
public:
	// typedef the classes to make our lives easier.
	typedef GraphArc<NodeType, ArcType> Arc;
	typedef GraphNode<NodeType, ArcType> Node;

	// Constructor function
	GraphNode() {}

    // Accessor functions
    std::list<Arc> const & arcList() const 
	{
        return m_arcList;              
    }

    bool marked(int npcID) const
	{
        return m_marked.at(npcID);
    }

	Node * getPrevious(int npcID) const
	{
		return m_previousList.at(npcID);
	}

    void setMarked(bool mark, int npcID)
	{
        m_marked.at(npcID) = mark;
    }
           
	void setPrevious(int npcID, Node *t_previous)
	{
		m_previousList.at(npcID) = t_previous;
	}

    Arc* getArc( Node* node );    
    void addArc( Node* node, ArcType weight );
    void removeArc( Node* node );

	// -------------------------------------------------------
	// Description: data stored in each node
	// -------------------------------------------------------
	NodeType m_data;

private:

	// -------------------------------------------------------
	// Description: list of arcs that the node has.
	// -------------------------------------------------------
	std::list<Arc> m_arcList;

	// -------------------------------------------------------
	// Description: This remembers if the node is marked.
	// -------------------------------------------------------
	std::vector<bool> m_marked = std::vector<bool>(m_npcCount, false);

	// -------------------------------------------------------
	// Description: pointer to previous node
	// -------------------------------------------------------
	std::vector<Node*> m_previousList = std::vector<Node*>(m_npcCount, nullptr);
};

// ----------------------------------------------------------------
//  Name:           getArc
//  Description:    This finds the arc in the current node that
//                  points to the node in the parameter.
//  Arguments:      The node that the arc connects to.
//  Return Value:   A pointer to the arc, or 0 if an arc doesn't
//                  exist from this to the specified input node.
// ----------------------------------------------------------------
template<typename NodeType, typename ArcType>
GraphArc<NodeType, ArcType>* GraphNode<NodeType, ArcType>::getArc( Node* node ) 
{

     auto iter = m_arcList.begin();
     auto endIter = m_arcList.end();
     Arc* arc = 0;
     
     // find the arc that matches the node
     for( ; iter != endIter && nullptr == arc; ++iter ) 
	 {         
          if ( (*iter).node() == node) 
		  {
               arc = &( (*iter) );
          }
     }

     // returns nullptr if not found
     return arc;
}


// ----------------------------------------------------------------
//  Name:           addArc
//  Description:    This adds an arc from the current node pointing
//                  to the first parameter, with the second parameter 
//                  as the weight.
//  Arguments:      First argument is the node to connect the arc to.
//                  Second argument is the weight of the arc.
//  Return Value:   None.
// ----------------------------------------------------------------
template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::addArc( Node* node, ArcType weight ) 
{
   // Create a new arc.
   Arc a;
   a.setNode(node);
   a.setWeight(weight);   
   // Add it to the arc list.
   m_arcList.push_back( a );
}


// ----------------------------------------------------------------
//  Name:           removeArc
//  Description:    This finds an arc from this node to input node 
//                  and removes it.
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::removeArc( Node* node ) 
{
     auto iter = m_arcList.begin();
     auto endIter = m_arcList.end();

     int size = m_arcList.size();
     // find the arc that matches the node
     for( ; iter != endIter && m_arcList.size() == size; ++iter ) 
	 {
          if ( (*iter).node() == node) 
		  {
             m_arcList.remove( (*iter) );
          }                           
     }
}

#include "GraphArc.h"

#endif
