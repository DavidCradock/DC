#pragma once
#include "../DCMath/AABB.h"
#include "octTreeEntity.h"
#include "../DCMath/frustum.h"
#include <map>

namespace DC
{
	class OctTree;

	// A node used by the OctTree class
	class OctTreeNode
	{
		friend class OctTree;
	public:
		// Used for accessing child nodes based upon their location
		enum ChildNode
		{
			NEGX_NEGY_NEGZ,
			NEGX_POSY_NEGZ,
			NEGX_NEGY_POSZ,
			NEGX_POSY_POSZ,
			POSX_NEGY_NEGZ,
			POSX_POSY_NEGZ,
			POSX_NEGY_POSZ,
			POSX_POSY_POSZ,
			NONE
		};

		// Constructor.
		// Sets up the node to represent the given region within the 3D world, with no child nodes.
		// parentNode is a pointer to this node's parent node. However, if this node is to represent
		// the root node, this will be 0.
		OctTreeNode(const AABB& region, OctTreeNode* parentNode, OctTree* octTree);

		// Destructor
		// Removes all child nodes
		// Although this obviously removes the entities from the nodes, because the nodes themselves
		// no longer exist, this does NOT delete the entity pointers. They are stored in the OctTree's
		// _mmapEntities hashmap.
		~OctTreeNode();

		
		// Debug renders this node and it's child nodes', node boundaries
		// line is the CResourceLine object which is being used to add vertices to be rendered
		// vertex is the vertex object we're using to add vertices using the pLine object.
//		void debugRenderNodes(CResourceVertexBufferLine* line, Colour colour) const;

		/*
		// Debug renders this node and it's child nodes' entities
		// line is the CResourceLine object which is being used to add vertices to be rendered
		void debugRenderEntities(CResourceLine* line, int iCircleRadius, unsigned int uiCircleNumSegments) const;
		*/

		// Returns true if this node has the specified child node
		bool hasChildNode(ChildNode childNode) const;

		// Returns whether this node has any child nodes
		bool hasAnyChildNodes(void) const;

		// Returns whether this node and all it's children have any entities within
		bool hasEntitiesInThisAndAllChildren(void) const;

		// Creates the specified child node
		// If the node already exists, this does nothing.
		void createChildNode(ChildNode childNode);

		// Calculates and returns a child's region.
		AABB computeChildNodeRegion(ChildNode childNode) const;

		// Adds an entity into this node, or it's children
		void addEntity(OctTreeEntity* entity);

		// Removes an entity from this node
		// If the entity couldn't be found, an exception occurs
		void removeEntity(OctTreeEntity* entity);

		// Adds nodes to a vector of COctTreeNodes which have entities in them
		void getNodesWithEntities(std::vector<OctTreeNode*>& nodes);

		// Adds nodes to a vector of COctTreeNodes which intersect with the given AABB and have entities
		void getNodesWithEntitiesWhichIntersect(std::vector<OctTreeNode*>& nodes, const AABB& aabb);

		// Adds nodes to a vector of COctTreeNodes which intersect with the given Frustum and have entities
		void getNodesWithEntitiesWhichIntersect(std::vector<OctTreeNode*>& nodes, const Frustum& frustum);

		// Go through all children and if their depth is greater, increases given uiMaxNodeDepth
		void getMaxNodeDepth(unsigned int& maxNodeDepth);
	private:
		// Holds the region which this node covers
		// Must be a multiple of 2, otherwise child nodes' regions will not cover all space.
		AABB _mRegion;

		// Pointer to the parent of this node. May be 0 if this is the root node
		OctTreeNode* _mpParentNode;

		// Pointer to the eight possible child nodes.
		// A pointer may be 0 for no child node allocated yet.
		// Use the ChildNode enum with this array to access the correct child node.
		OctTreeNode* _mpChildNode[8];

		// The quad tree which owns this node, this is passed to the constructor
		OctTree* _mpOctTree;

		// Depth of this node.
		// How many nodes there are above this node.
		unsigned int _muiNodeDepth;

		// Hashmap holding pointers to each of the added entities, until
		// this node has children, in which case this would be empty as 
		// the child nodes now own the entities (or their siblings)
		mutable std::map<std::wstring, OctTreeEntity*> _mmapEntities;
	};
}
