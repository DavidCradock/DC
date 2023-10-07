#include "quadTreeNode.h"
#include "quadTree.h"
#include "../DCCommon/error.h"

namespace DC
{
	QuadTreeNode::QuadTreeNode(const Rect& rectRegionPARAM, QuadTreeNode* parentNodePARAM, QuadTree* quadTreePARAM)
	{
		rectRegion = rectRegionPARAM;
		parentNode = parentNodePARAM;
		quadTree = quadTreePARAM;
		// Is this the root node?
		if (!parentNodePARAM)
			nodeDepth = 0;
		else
			nodeDepth = parentNodePARAM->nodeDepth + 1;

		if (nodeDepth > quadTreePARAM->currentMaxNodeDepth)
			quadTreePARAM->currentMaxNodeDepth = nodeDepth;

		// No children yet
		for (int i = 0; i < 4; i++)
			childNodes[i] = 0;
	}

	QuadTreeNode::~QuadTreeNode()
	{
		// Destroy each child node
		// This will recursively call each child node's destructor
		for (int i = 0; i < 4; i++)
		{
			// If child node exists
			if (childNodes[i])
				delete childNodes[i];
		}
	}
/*
	void QuadTreeNode::debugRenderNodes(CResourceVertexBufferLine* line, CResourceVertexBufferLine::Vertex& vertex, Colour colour) const
	{
		// Compute colour
		float fDepthCol = (float)_mpQuadTree->_muiMaxNodeDepth - float(_muiNodeDepth);
		fDepthCol /= (float)_mpQuadTree->_muiMaxNodeDepth;
		clamp(fDepthCol, 0.1f, 1.0f);
		vertex.colour = colour;
		vertex.colour.alpha = fDepthCol;

		// Render this node's four edges
		// Left
		vertex.position.set((float)_mRectRegion.miMinX, (float)_mRectRegion.miMinY, 0.0f);	pLine->addLinePoint(vertex);
		vertex.position.set((float)_mRectRegion.miMinX, (float)_mRectRegion.miMaxY, 0.0f);	pLine->addLinePoint(vertex);
		// Right
		vertex.position.set((float)_mRectRegion.miMaxX, (float)_mRectRegion.miMinY, 0.0f);	pLine->addLinePoint(vertex);
		vertex.position.set((float)_mRectRegion.miMaxX, (float)_mRectRegion.miMaxY, 0.0f);	pLine->addLinePoint(vertex);
		// Top
		vertex.position.set((float)_mRectRegion.miMinX, (float)_mRectRegion.miMaxY, 0.0f);	pLine->addLinePoint(vertex);
		vertex.position.set((float)_mRectRegion.miMaxX, (float)_mRectRegion.miMaxY, 0.0f);	pLine->addLinePoint(vertex);
		// Bottom
		vertex.position.set((float)_mRectRegion.miMinX, (float)_mRectRegion.miMinY, 0.0f);	pLine->addLinePoint(vertex);
		vertex.position.set((float)_mRectRegion.miMaxX, (float)_mRectRegion.miMinY, 0.0f);	pLine->addLinePoint(vertex);

		// Render child nodes if they exist
		if (_mpChildNode[ChildNode::BL])
		{
			colour.set(0, 0, 1, 1);
			_mpChildNode[ChildNode::BL]->debugRenderNodes(pLine, vertex, colour);
		}
		if (_mpChildNode[ChildNode::BR])
		{
			colour.set(1, 1, 0, 1);
			_mpChildNode[ChildNode::BR]->debugRenderNodes(pLine, vertex, colour);
		}
		if (_mpChildNode[ChildNode::TL])
		{
			colour.set(1, 0, 0, 1);
			_mpChildNode[ChildNode::TL]->debugRenderNodes(pLine, vertex, colour);
		}
		if (_mpChildNode[ChildNode::TR])
		{
			colour.set(0, 1, 0, 1);
			_mpChildNode[ChildNode::TR]->debugRenderNodes(pLine, vertex, colour);
		}
	}

	void QuadTreeNode::debugRenderEntities(CResourceVertexBufferLine* pLine, int iCircleRadius, unsigned int uiCircleNumSegments) const
	{
		// For each entity in this node
		std::map<std::wstring, QuadTreeEntity*>::iterator it = _mmapEntities.begin();
		while (it != _mmapEntities.end())
		{
			pLine->addCircle(it->second->_miPosX, it->second->_miPosY, (float)iCircleRadius, uiCircleNumSegments, it->second->_mColDebug);
			it++;
		}

		// Render child nodes if they exist
		for (int i = 0; i < 4; i++)
		{
			if (_mpChildNode[i])
				_mpChildNode[i]->debugRenderEntities(pLine, iCircleRadius, uiCircleNumSegments);
		}
	}
*/
	bool QuadTreeNode::hasChildNode(ChildNode childNodePARAM) const
	{
		return childNodes[childNodePARAM];
	}

	bool QuadTreeNode::hasAnyChildNodes(void) const
	{
		for (int i = 0; i < 4; i++)
		{
			if (childNodes[i])
				return true;
		}
		return false;
	}

	bool QuadTreeNode::hasEntitiesInThisAndAllChildren(void) const
	{
		// If this node has entities in
		if (entities.size() > 0)
			return true;

		// Now recursively go through any existing child nodes
		for (int i = 0; i < 4; i++)
		{
			// If node exists
			if (childNodes[i])
			{
				if (childNodes[i]->hasEntitiesInThisAndAllChildren())
					return true;
			}
		}

		// If we get here, this node has no entities and neither does any of it's children nodes
		return false;
	}

	void QuadTreeNode::createChildNode(ChildNode childNodePARAM)
	{
		if (hasChildNode(childNodePARAM))
			return;

		// Compute rect area of the new child node
		Rect childNodeRect = computeChildNodeRegion(childNodePARAM);

		childNodes[childNodePARAM] = new QuadTreeNode(childNodeRect, this, quadTree);
		ErrorIfFalse(childNodes[childNodePARAM], L"QuadTreeNode::createChildNode() failed. Unable to allocate memory for the new child node.");
	}

	Rect QuadTreeNode::computeChildNodeRegion(ChildNode childNodePARAM) const
	{
		// Compute dimensions of new child node using the rect region of this parent node.
		int iChildDimsX = rectRegion.maxX - rectRegion.minX;
		int iChildDimsY = rectRegion.maxY - rectRegion.minY;
		iChildDimsX /= 2;
		iChildDimsY /= 2;

		// Calculate region of the child node
		Rect childNodeRegion;
		switch (childNodePARAM)
		{
		case ChildNode::BL:
			childNodeRegion.minX = rectRegion.minX;
			childNodeRegion.maxX = rectRegion.minX + iChildDimsX;
			childNodeRegion.minY = rectRegion.minY;
			childNodeRegion.maxY = rectRegion.minY + iChildDimsY;
			break;
		case ChildNode::BR:
			childNodeRegion.minX = rectRegion.minX + iChildDimsX;
			childNodeRegion.maxX = rectRegion.maxX;
			childNodeRegion.minY = rectRegion.minY;
			childNodeRegion.maxY = rectRegion.minY + iChildDimsY;
			break;
		case ChildNode::TL:
			childNodeRegion.minX = rectRegion.minX;
			childNodeRegion.maxX = rectRegion.minX + iChildDimsX;
			childNodeRegion.minY = rectRegion.minY + iChildDimsY;
			childNodeRegion.maxY = rectRegion.maxY;
			break;
		case ChildNode::TR:
			childNodeRegion.minX = rectRegion.minX + iChildDimsX;
			childNodeRegion.maxX = rectRegion.maxX;
			childNodeRegion.minY = rectRegion.minY + iChildDimsY;
			childNodeRegion.maxY = rectRegion.maxY;
			break;
		default:
			ErrorIfTrue(1, L"QuadTreeNode::computeChildNodeRegion() given invalid ChildNode to compute it's region.");
		}
		return childNodeRegion;
	}

	void QuadTreeNode::addEntity(QuadTreeEntity* entityPARAM)
	{
		// If this node has no children, attempt to add the entity to this node
		if (!hasAnyChildNodes())
		{
			// We haven't reached max capacity for this node
			// OR we've reached maximum node depth with this node
			if (entities.size() < quadTree->maxEntitiesPerNode ||
				nodeDepth == quadTree->maxNodeDepth)
			{
				// Add the entity to this node
				// No need to check if the new entity name already exists, as QuadTree::addEntity() has already checked
				entities[entityPARAM->name] = entityPARAM;
				entityPARAM->nodeOwner = this;	// Set node owner for the entity
				return;
			}
			// If we've reached maximum capacity for this node and max node depth hasn't been reached
			if (entities.size() == quadTree->maxEntitiesPerNode)
			{
				// We need to create child node/s then move all the entities from this node into the children, 
				// as well as the new entity

				// Compute potential child node rects
				Rect rctBL = computeChildNodeRegion(ChildNode::BL);
				Rect rctBR = computeChildNodeRegion(ChildNode::BR);
				Rect rctTL = computeChildNodeRegion(ChildNode::TL);
				Rect rctTR = computeChildNodeRegion(ChildNode::TR);

				// Add the new entity to this node's entity map, so it'll be moved to the correct child node below
				entities[entityPARAM->name] = entityPARAM;
				entityPARAM->nodeOwner = this;	// Set node owner for the entity

				// Move all the entities from this node, into the child nodes
				std::map<std::wstring, QuadTreeEntity*>::iterator it = entities.begin();
				while (it != entities.end())
				{
					// Determine which child node the entity fits in, regardless of whether the child node exists or not
					ChildNode childNode = ChildNode::NONE;
					if (rctBL.doesPositionFitWithin(it->second->positionX, it->second->positionY))
						childNode = ChildNode::BL;
					else if (rctBR.doesPositionFitWithin(it->second->positionX, it->second->positionY))
						childNode = ChildNode::BR;
					else if (rctTL.doesPositionFitWithin(it->second->positionX, it->second->positionY))
						childNode = ChildNode::TL;
					else if (rctTR.doesPositionFitWithin(it->second->positionX, it->second->positionY))
						childNode = ChildNode::TR;

					// Error checking, making sure the entity could fit in one of the four possible children
					ErrorIfTrue(ChildNode::NONE == childNode, L"QuadTreeNode::addEntity() failed when trying to add entity " + it->second->name + L" to any of the four child nodes as it's position doesn't fit inside any of them.");

					// Create the child node if it doesn't exist
					if (!childNodes[childNode])
					{
						childNodes[childNode] = new QuadTreeNode(computeChildNodeRegion(childNode), this, quadTree);
						ErrorIfFalse(childNodes[childNode], L"QuadTreeNode::addEntity() failed when trying to add entity " + it->second->name + L" to any of the four child nodes as it was unable to allocate memory for the child node.");
					}
					childNodes[childNode]->addEntity(it->second);
					it++;
				}
				// Remove all entities from this node as they are now stored in the children
				entities.clear();
				return;
			}
		}

		// If we get here, then this node has children, so add the new entity to one of those...
		// Determine which child node the entity fits in
		// Compute potential child node rects
		Rect rctBL = computeChildNodeRegion(ChildNode::BL);
		Rect rctBR = computeChildNodeRegion(ChildNode::BR);
		Rect rctTL = computeChildNodeRegion(ChildNode::TL);
		Rect rctTR = computeChildNodeRegion(ChildNode::TR);

		ChildNode childNode = ChildNode::NONE;
		if (rctBL.doesPositionFitWithin(entityPARAM->positionX, entityPARAM->positionY))
			childNode = ChildNode::BL;
		else if (rctBR.doesPositionFitWithin(entityPARAM->positionX, entityPARAM->positionY))
			childNode = ChildNode::BR;
		else if (rctTL.doesPositionFitWithin(entityPARAM->positionX, entityPARAM->positionY))
			childNode = ChildNode::TL;
		else if (rctTR.doesPositionFitWithin(entityPARAM->positionX, entityPARAM->positionY))
			childNode = ChildNode::TR;
		// Error checking, making sure the entity could fit in one of the four possible children
		ErrorIfTrue(ChildNode::NONE == childNode, L"QuadTreeNode::addEntity() failed when trying to add entity " + entityPARAM->name + L" to any of the four child nodes as it's position doesn't fit inside any of them.");

		// Create the child node if it doesn't exist
		if (!childNodes[childNode])
		{
			childNodes[childNode] = new QuadTreeNode(computeChildNodeRegion(childNode), this, quadTree);
			ErrorIfFalse(childNodes[childNode], L"QuadTreeNode::addEntity() failed when trying to add entity " + entityPARAM->name + L" to any of the four child nodes as it was unable to allocate memory for the child node.");
		}

		// Add the new entity
		childNodes[childNode]->addEntity(entityPARAM);
	}

	void QuadTreeNode::removeEntity(QuadTreeEntity* entityPARAM)
	{
		// Attempt to find the entity
		std::map<std::wstring, QuadTreeEntity*>::iterator it = entities.find(entityPARAM->name);
		ErrorIfTrue(it == entities.end(), L"QuadTreeNode::removeEntity() failed. The entity named " + entityPARAM->name + L" could not be found");
		entities.erase(it);
		// No need to delete entity, the QuadTree::removeAllEntities() or QuadTree::removeEntity() does this
	}

	void QuadTreeNode::getNodesWithEntities(std::vector<QuadTreeNode*>& nodesPARAM)
	{
		// If this node doesn't have any children, just check this node
		if (!hasAnyChildNodes())
		{
			// If this node doesn't have any entities
			if (0 == entities.size())
				return;

			// Add this node
			nodesPARAM.push_back(this);
			return;
		}
		else  // This node has children, check those
		{
			for (int i = 0; i < 4; i++)
			{
				if (childNodes[i])
					childNodes[i]->getNodesWithEntities(nodesPARAM);
			}
		}
	}

	void QuadTreeNode::getNodesWithEntitiesWhichIntersect(std::vector<QuadTreeNode*>& nodesPARAM, const Rect& rectPARAM)
	{
		// If this node doesn't have any children, just check this node
		if (!hasAnyChildNodes())
		{
			// If this node doesn't have any entities
			if (0 == entities.size())
				return;

			// If the rect interects with this node, add this node
			if (rectRegion.intersects(rectPARAM))
			{
				nodesPARAM.push_back(this);
				return;
			}
		}
		else  // This node has children, check those
		{
			for (int i = 0; i < 4; i++)
			{
				if (childNodes[i])
					childNodes[i]->getNodesWithEntitiesWhichIntersect(nodesPARAM, rectPARAM);
			}
		}
	}

	void QuadTreeNode::getMaxNodeDepth(unsigned int& maxNodeDepthPARAM)
	{
		if (nodeDepth > maxNodeDepthPARAM)
			maxNodeDepthPARAM = nodeDepth;

		// Call this method for all children of this node
		for (int i = 0; i < 4; i++)
		{
			if (childNodes[i])	// If child exists
				childNodes[i]->getMaxNodeDepth(maxNodeDepthPARAM);
		}
	}
}