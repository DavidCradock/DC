#include "quadTreeNode.h"
#include "quadTree.h"
#include "../DCCommon/error.h"

namespace DC
{
	QuadTreeNode::QuadTreeNode(const Rect& rectRegionPARAM, QuadTreeNode* parentNodePARAM, QuadTree* quadTreePARAM)
	{
		_mRectRegion = rectRegionPARAM;
		_mpParentNode = parentNodePARAM;
		_mpQuadTree = quadTreePARAM;
		// Is this the root node?
		if (!parentNodePARAM)
			_muiNodeDepth = 0;
		else
			_muiNodeDepth = parentNodePARAM->_muiNodeDepth + 1;

		if (_muiNodeDepth > quadTreePARAM->_muiCurrentMaxNodeDepth)
			quadTreePARAM->_muiCurrentMaxNodeDepth = _muiNodeDepth;

		// No children yet
		for (int i = 0; i < 4; i++)
			_mpChildNode[i] = 0;
	}

	QuadTreeNode::~QuadTreeNode()
	{
		// Destroy each child node
		// This will recursively call each child node's destructor
		for (int i = 0; i < 4; i++)
		{
			// If child node exists
			if (_mpChildNode[i])
				delete _mpChildNode[i];
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
		return _mpChildNode[childNodePARAM];
	}

	bool QuadTreeNode::hasAnyChildNodes(void) const
	{
		for (int i = 0; i < 4; i++)
		{
			if (_mpChildNode[i])
				return true;
		}
		return false;
	}

	bool QuadTreeNode::hasEntitiesInThisAndAllChildren(void) const
	{
		// If this node has entities in
		if (_mmapEntities.size() > 0)
			return true;

		// Now recursively go through any existing child nodes
		for (int i = 0; i < 4; i++)
		{
			// If node exists
			if (_mpChildNode[i])
			{
				if (_mpChildNode[i]->hasEntitiesInThisAndAllChildren())
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

		_mpChildNode[childNodePARAM] = new QuadTreeNode(childNodeRect, this, _mpQuadTree);
		ErrorIfFalse(_mpChildNode[childNodePARAM], L"QuadTreeNode::createChildNode() failed. Unable to allocate memory for the new child node.");
	}

	Rect QuadTreeNode::computeChildNodeRegion(ChildNode childNodePARAM) const
	{
		// Compute dimensions of new child node using the rect region of this parent node.
		int iChildDimsX = _mRectRegion.maxX - _mRectRegion.minX;
		int iChildDimsY = _mRectRegion.maxY - _mRectRegion.minY;
		iChildDimsX /= 2;
		iChildDimsY /= 2;

		// Calculate region of the child node
		Rect childNodeRegion;
		switch (childNodePARAM)
		{
		case ChildNode::BL:
			childNodeRegion.minX = _mRectRegion.minX;
			childNodeRegion.maxX = _mRectRegion.minX + iChildDimsX;
			childNodeRegion.minY = _mRectRegion.minY;
			childNodeRegion.maxY = _mRectRegion.minY + iChildDimsY;
			break;
		case ChildNode::BR:
			childNodeRegion.minX = _mRectRegion.minX + iChildDimsX;
			childNodeRegion.maxX = _mRectRegion.maxX;
			childNodeRegion.minY = _mRectRegion.minY;
			childNodeRegion.maxY = _mRectRegion.minY + iChildDimsY;
			break;
		case ChildNode::TL:
			childNodeRegion.minX = _mRectRegion.minX;
			childNodeRegion.maxX = _mRectRegion.minX + iChildDimsX;
			childNodeRegion.minY = _mRectRegion.minY + iChildDimsY;
			childNodeRegion.maxY = _mRectRegion.maxY;
			break;
		case ChildNode::TR:
			childNodeRegion.minX = _mRectRegion.minX + iChildDimsX;
			childNodeRegion.maxX = _mRectRegion.maxX;
			childNodeRegion.minY = _mRectRegion.minY + iChildDimsY;
			childNodeRegion.maxY = _mRectRegion.maxY;
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
			if (_mmapEntities.size() < _mpQuadTree->_miMaxEntitiesPerNode ||
				_muiNodeDepth == _mpQuadTree->_muiMaxNodeDepth)
			{
				// Add the entity to this node
				// No need to check if the new entity name already exists, as QuadTree::addEntity() has already checked
				_mmapEntities[entityPARAM->_mstrName] = entityPARAM;
				entityPARAM->_mpNodeOwner = this;	// Set node owner for the entity
				return;
			}
			// If we've reached maximum capacity for this node and max node depth hasn't been reached
			if (_mmapEntities.size() == _mpQuadTree->_miMaxEntitiesPerNode)
			{
				// We need to create child node/s then move all the entities from this node into the children, 
				// as well as the new entity

				// Compute potential child node rects
				Rect rctBL = computeChildNodeRegion(ChildNode::BL);
				Rect rctBR = computeChildNodeRegion(ChildNode::BR);
				Rect rctTL = computeChildNodeRegion(ChildNode::TL);
				Rect rctTR = computeChildNodeRegion(ChildNode::TR);

				// Add the new entity to this node's entity map, so it'll be moved to the correct child node below
				_mmapEntities[entityPARAM->_mstrName] = entityPARAM;
				entityPARAM->_mpNodeOwner = this;	// Set node owner for the entity

				// Move all the entities from this node, into the child nodes
				std::map<std::wstring, QuadTreeEntity*>::iterator it = _mmapEntities.begin();
				while (it != _mmapEntities.end())
				{
					// Determine which child node the entity fits in, regardless of whether the child node exists or not
					ChildNode childNode = ChildNode::NONE;
					if (rctBL.doesPositionFitWithin(it->second->_miPosX, it->second->_miPosY))
						childNode = ChildNode::BL;
					else if (rctBR.doesPositionFitWithin(it->second->_miPosX, it->second->_miPosY))
						childNode = ChildNode::BR;
					else if (rctTL.doesPositionFitWithin(it->second->_miPosX, it->second->_miPosY))
						childNode = ChildNode::TL;
					else if (rctTR.doesPositionFitWithin(it->second->_miPosX, it->second->_miPosY))
						childNode = ChildNode::TR;

					// Error checking, making sure the entity could fit in one of the four possible children
					ErrorIfTrue(ChildNode::NONE == childNode, L"QuadTreeNode::addEntity() failed when trying to add entity " + it->second->_mstrName + L" to any of the four child nodes as it's position doesn't fit inside any of them.");

					// Create the child node if it doesn't exist
					if (!_mpChildNode[childNode])
					{
						_mpChildNode[childNode] = new QuadTreeNode(computeChildNodeRegion(childNode), this, _mpQuadTree);
						ErrorIfFalse(_mpChildNode[childNode], L"QuadTreeNode::addEntity() failed when trying to add entity " + it->second->_mstrName + L" to any of the four child nodes as it was unable to allocate memory for the child node.");
					}
					_mpChildNode[childNode]->addEntity(it->second);
					it++;
				}
				// Remove all entities from this node as they are now stored in the children
				_mmapEntities.clear();
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
		if (rctBL.doesPositionFitWithin(entityPARAM->_miPosX, entityPARAM->_miPosY))
			childNode = ChildNode::BL;
		else if (rctBR.doesPositionFitWithin(entityPARAM->_miPosX, entityPARAM->_miPosY))
			childNode = ChildNode::BR;
		else if (rctTL.doesPositionFitWithin(entityPARAM->_miPosX, entityPARAM->_miPosY))
			childNode = ChildNode::TL;
		else if (rctTR.doesPositionFitWithin(entityPARAM->_miPosX, entityPARAM->_miPosY))
			childNode = ChildNode::TR;
		// Error checking, making sure the entity could fit in one of the four possible children
		ErrorIfTrue(ChildNode::NONE == childNode, L"QuadTreeNode::addEntity() failed when trying to add entity " + entityPARAM->_mstrName + L" to any of the four child nodes as it's position doesn't fit inside any of them.");

		// Create the child node if it doesn't exist
		if (!_mpChildNode[childNode])
		{
			_mpChildNode[childNode] = new QuadTreeNode(computeChildNodeRegion(childNode), this, _mpQuadTree);
			ErrorIfFalse(_mpChildNode[childNode], L"QuadTreeNode::addEntity() failed when trying to add entity " + entityPARAM->_mstrName + L" to any of the four child nodes as it was unable to allocate memory for the child node.");
		}

		// Add the new entity
		_mpChildNode[childNode]->addEntity(entityPARAM);
	}

	void QuadTreeNode::removeEntity(QuadTreeEntity* entityPARAM)
	{
		// Attempt to find the entity
		std::map<std::wstring, QuadTreeEntity*>::iterator it = _mmapEntities.find(entityPARAM->_mstrName);
		ErrorIfTrue(it == _mmapEntities.end(), L"QuadTreeNode::removeEntity() failed. The entity named " + entityPARAM->_mstrName + L" could not be found");
		_mmapEntities.erase(it);
		// No need to delete entity, the QuadTree::removeAllEntities() or QuadTree::removeEntity() does this
	}

	void QuadTreeNode::getNodesWithEntities(std::vector<QuadTreeNode*>& nodesPARAM)
	{
		// If this node doesn't have any children, just check this node
		if (!hasAnyChildNodes())
		{
			// If this node doesn't have any entities
			if (0 == _mmapEntities.size())
				return;

			// Add this node
			nodesPARAM.push_back(this);
			return;
		}
		else  // This node has children, check those
		{
			for (int i = 0; i < 4; i++)
			{
				if (_mpChildNode[i])
					_mpChildNode[i]->getNodesWithEntities(nodesPARAM);
			}
		}
	}

	void QuadTreeNode::getNodesWithEntitiesWhichIntersect(std::vector<QuadTreeNode*>& nodesPARAM, const Rect& rectPARAM)
	{
		// If this node doesn't have any children, just check this node
		if (!hasAnyChildNodes())
		{
			// If this node doesn't have any entities
			if (0 == _mmapEntities.size())
				return;

			// If the rect interects with this node, add this node
			if (_mRectRegion.intersects(rectPARAM))
			{
				nodesPARAM.push_back(this);
				return;
			}
		}
		else  // This node has children, check those
		{
			for (int i = 0; i < 4; i++)
			{
				if (_mpChildNode[i])
					_mpChildNode[i]->getNodesWithEntitiesWhichIntersect(nodesPARAM, rectPARAM);
			}
		}
	}

	void QuadTreeNode::getMaxNodeDepth(unsigned int& maxNodeDepthPARAM)
	{
		if (_muiNodeDepth > maxNodeDepthPARAM)
			maxNodeDepthPARAM = _muiNodeDepth;

		// Call this method for all children of this node
		for (int i = 0; i < 4; i++)
		{
			if (_mpChildNode[i])	// If child exists
				_mpChildNode[i]->getMaxNodeDepth(maxNodeDepthPARAM);
		}
	}
}