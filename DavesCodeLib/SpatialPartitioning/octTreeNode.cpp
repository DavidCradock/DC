#include "octTreeNode.h"
#include "octTree.h"
#include "../Common/error.h"

namespace DC
{
	OctTreeNode::OctTreeNode(const AABB& regionPARAM, OctTreeNode* parentNodePARAM, OctTree* octTreePARAM)
	{
		region = regionPARAM;
		parentNode = parentNodePARAM;
		octTree = octTreePARAM;
		// Is this the root node?
		if (!parentNodePARAM)
			nodeDepth = 0;
		else
			nodeDepth = parentNodePARAM->nodeDepth + 1;

		if (nodeDepth > octTreePARAM->currentMaxNodeDepth)
			octTreePARAM->currentMaxNodeDepth = nodeDepth;

		// No children yet
		for (int i = 0; i < 8; i++)
			childNodes[i] = 0;
	}

	OctTreeNode::~OctTreeNode()
	{
		// Destroy each child node
		// This will recursively call each child node's destructor
		for (int i = 0; i < 8; i++)
		{
			// If child node exists
			if (childNodes[i])
				delete childNodes[i];
		}
	}

/*
	void OctTreeNode::debugRenderNodes(CResourceVertexBufferLine* line, Colour colour) const
	{
		// Compute colour
		float fDepthCol = (float)_mpOctTree->_muiMaxNodeDepth - float(_muiNodeDepth);
		fDepthCol /= (float)_mpOctTree->_muiMaxNodeDepth;
		clamp(fDepthCol, 0.75f, 1.0f);

		// Compute vertex positions for the AABB
		CResourceVertexBufferLine::Vertex v[8];
		for (int i = 0; i < 8; i++)
		{
			v[i].colour.red = colour.red;
			v[i].colour.green = colour.green;
			v[i].colour.blue = colour.blue;
			v[i].colour.alpha = fDepthCol;
		}
		v[ChildNode::NEGX_NEGY_NEGZ].position = _mRegion.getMin();
		v[ChildNode::POSX_POSY_POSZ].position = _mRegion.getMax();
		v[ChildNode::NEGX_NEGY_POSZ].position.set(v[ChildNode::NEGX_NEGY_NEGZ].position.x, v[ChildNode::NEGX_NEGY_NEGZ].position.y, v[ChildNode::POSX_POSY_POSZ].position.z);
		v[ChildNode::NEGX_POSY_NEGZ].position.set(v[ChildNode::NEGX_NEGY_NEGZ].position.x, v[ChildNode::POSX_POSY_POSZ].position.y, v[ChildNode::NEGX_NEGY_NEGZ].position.z);
		v[ChildNode::NEGX_POSY_POSZ].position.set(v[ChildNode::NEGX_NEGY_NEGZ].position.x, v[ChildNode::POSX_POSY_POSZ].position.y, v[ChildNode::POSX_POSY_POSZ].position.z);
		v[ChildNode::POSX_NEGY_NEGZ].position.set(v[ChildNode::POSX_POSY_POSZ].position.x, v[ChildNode::NEGX_NEGY_NEGZ].position.y, v[ChildNode::NEGX_NEGY_NEGZ].position.z);
		v[ChildNode::POSX_NEGY_POSZ].position.set(v[ChildNode::POSX_POSY_POSZ].position.x, v[ChildNode::NEGX_NEGY_NEGZ].position.y, v[ChildNode::POSX_POSY_POSZ].position.z);
		v[ChildNode::POSX_POSY_NEGZ].position.set(v[ChildNode::POSX_POSY_POSZ].position.x, v[ChildNode::POSX_POSY_POSZ].position.y, v[ChildNode::NEGX_NEGY_NEGZ].position.z);

		// Render this node's twelve edges
		pLine->addLinePoint(v[NEGX_NEGY_NEGZ]);	pLine->addLinePoint(v[POSX_NEGY_NEGZ]);		// Back, bottom, left to right
		pLine->addLinePoint(v[NEGX_POSY_NEGZ]);	pLine->addLinePoint(v[POSX_POSY_NEGZ]);		// Back, top, left to right
		pLine->addLinePoint(v[NEGX_NEGY_POSZ]);	pLine->addLinePoint(v[POSX_NEGY_POSZ]);		// Front, bottom, left to right
		pLine->addLinePoint(v[NEGX_POSY_POSZ]);	pLine->addLinePoint(v[POSX_POSY_POSZ]);		// Front, top, left to right
		pLine->addLinePoint(v[NEGX_POSY_NEGZ]);	pLine->addLinePoint(v[NEGX_NEGY_NEGZ]);		// Back, left, top to bottom
		pLine->addLinePoint(v[POSX_POSY_NEGZ]);	pLine->addLinePoint(v[POSX_NEGY_NEGZ]);		// Back, right, top to bottom
		pLine->addLinePoint(v[NEGX_POSY_POSZ]);	pLine->addLinePoint(v[NEGX_NEGY_POSZ]);		// Front, left, top to bottom
		pLine->addLinePoint(v[POSX_POSY_POSZ]);	pLine->addLinePoint(v[POSX_NEGY_POSZ]);		// Front, right, top to bottom
		pLine->addLinePoint(v[NEGX_POSY_POSZ]);	pLine->addLinePoint(v[NEGX_POSY_NEGZ]);		// Top, left, front to back
		pLine->addLinePoint(v[POSX_POSY_POSZ]);	pLine->addLinePoint(v[POSX_POSY_NEGZ]);		// Top, right, front to back
		pLine->addLinePoint(v[NEGX_NEGY_POSZ]);	pLine->addLinePoint(v[NEGX_NEGY_NEGZ]);		// Bottom, left, front to back
		pLine->addLinePoint(v[POSX_NEGY_POSZ]);	pLine->addLinePoint(v[POSX_NEGY_NEGZ]);		// Bottom, right, front to back

		// Render child nodes if they exist
		for (int i = 0; i < 8; i++)
		{
			if (_mpChildNode[i])
				_mpChildNode[i]->debugRenderNodes(pLine, colour);
		}
	}
*/

	/*
	void OctTreeNode::debugRenderEntities(CResourceLine* line, int iCircleRadius, unsigned int circleNumSegments) const
	{
		// For each entity in this node
		std::map<std::wstring, OctTreeEntity*>::iterator it = _mmapEntities.begin();
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

	bool OctTreeNode::hasChildNode(ChildNode childNodePARAM) const
	{
		return childNodes[childNodePARAM];
	}

	bool OctTreeNode::hasAnyChildNodes(void) const
	{
		for (int i = 0; i < 8; i++)
		{
			if (childNodes[i])
				return true;
		}
		return false;
	}

	bool OctTreeNode::hasEntitiesInThisAndAllChildren(void) const
	{
		// If this node has entities in
		if (entities.size() > 0)
			return true;

		// Now recursively go through any existing child nodes
		for (int i = 0; i < 8; i++)
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

	void OctTreeNode::createChildNode(ChildNode childNodePARAM)
	{
		if (hasChildNode(childNodePARAM))
			return;

		// Compute region of the new child node
		AABB childNodeRegion = computeChildNodeRegion(childNodePARAM);

		childNodes[childNodePARAM] = new OctTreeNode(childNodeRegion, this, octTree);
		ErrorIfFalse(childNodes[childNodePARAM], L"OctTreeNode::createChildNode() failed. Unable to allocate memory for the new child node.");
	}

	AABB OctTreeNode::computeChildNodeRegion(ChildNode childNodePARAM) const
	{
		// Compute dimensions of new child node using the region of this parent node.
		Vector3f vChildDims = region.getHalfDimensions();
		Vector3f vParentPosition = region.getPosition();
		Vector3f vHalfChildDims = vChildDims * 0.5f;

		// Calculate region of the child node
		AABB childNodeRegion;
		Vector3f vChildPosition = vParentPosition;
		switch (childNodePARAM)
		{
		case ChildNode::NEGX_NEGY_NEGZ:
			vChildPosition.x -= vHalfChildDims.x;
			vChildPosition.y -= vHalfChildDims.y;
			vChildPosition.z -= vHalfChildDims.z;
			childNodeRegion.setPosDims(vChildPosition, vChildDims);
			break;
		case ChildNode::NEGX_POSY_NEGZ:
			vChildPosition.x -= vHalfChildDims.x;
			vChildPosition.y += vHalfChildDims.y;
			vChildPosition.z -= vHalfChildDims.z;
			childNodeRegion.setPosDims(vChildPosition, vChildDims);
			break;
		case ChildNode::NEGX_NEGY_POSZ:
			vChildPosition.x -= vHalfChildDims.x;
			vChildPosition.y -= vHalfChildDims.y;
			vChildPosition.z += vHalfChildDims.z;
			childNodeRegion.setPosDims(vChildPosition, vChildDims);
			break;
		case ChildNode::NEGX_POSY_POSZ:
			vChildPosition.x -= vHalfChildDims.x;
			vChildPosition.y += vHalfChildDims.y;
			vChildPosition.z += vHalfChildDims.z;
			childNodeRegion.setPosDims(vChildPosition, vChildDims);
			break;
		case ChildNode::POSX_NEGY_NEGZ:
			vChildPosition.x += vHalfChildDims.x;
			vChildPosition.y -= vHalfChildDims.y;
			vChildPosition.z -= vHalfChildDims.z;
			childNodeRegion.setPosDims(vChildPosition, vChildDims);
			break;
		case ChildNode::POSX_POSY_NEGZ:
			vChildPosition.x += vHalfChildDims.x;
			vChildPosition.y += vHalfChildDims.y;
			vChildPosition.z -= vHalfChildDims.z;
			childNodeRegion.setPosDims(vChildPosition, vChildDims);
			break;
		case ChildNode::POSX_NEGY_POSZ:
			vChildPosition.x += vHalfChildDims.x;
			vChildPosition.y -= vHalfChildDims.y;
			vChildPosition.z += vHalfChildDims.z;
			childNodeRegion.setPosDims(vChildPosition, vChildDims);
			break;
		case ChildNode::POSX_POSY_POSZ:
			vChildPosition.x += vHalfChildDims.x;
			vChildPosition.y += vHalfChildDims.y;
			vChildPosition.z += vHalfChildDims.z;
			childNodeRegion.setPosDims(vChildPosition, vChildDims);
			break;
		default:
			ErrorIfTrue(1, L"OctTreeNode::computeChildNodeRegion() given invalid ChildNode to compute it's region.");
		}
		return childNodeRegion;
	}

	void OctTreeNode::addEntity(OctTreeEntity* entityPARAM)
	{
		// If this node has no children, attempt to add the entity to this node
		if (!hasAnyChildNodes())
		{
			// We haven't reached max capacity for this node
			// OR we've reached maximum node depth with this node
			if (entities.size() < octTree->maxEntitiesPerNode ||
				nodeDepth == octTree->maxNodeDepth)
			{
				// Add the entity to this node
				// No need to check if the new entity name already exists, as OctTree::addEntity() has already checked
				entities[entityPARAM->name] = entityPARAM;
				entityPARAM->nodeOwner = this;	// Set node owner for the entity
				return;
			}
			// If we've reached maximum capacity for this node and max node depth hasn't been reached
			if (entities.size() == octTree->maxEntitiesPerNode)
			{
				// We need to create child node/s then move all the entities from this node into the children, 
				// as well as the new entity

				// Compute potential child node regions
				AABB aabbNNN = computeChildNodeRegion(ChildNode::NEGX_NEGY_NEGZ);
				AABB aabbNNP = computeChildNodeRegion(ChildNode::NEGX_NEGY_POSZ);
				AABB aabbNPN = computeChildNodeRegion(ChildNode::NEGX_POSY_NEGZ);
				AABB aabbNPP = computeChildNodeRegion(ChildNode::NEGX_POSY_POSZ);
				AABB aabbPNN = computeChildNodeRegion(ChildNode::POSX_NEGY_NEGZ);
				AABB aabbPNP = computeChildNodeRegion(ChildNode::POSX_NEGY_POSZ);
				AABB aabbPPN = computeChildNodeRegion(ChildNode::POSX_POSY_NEGZ);
				AABB aabbPPP = computeChildNodeRegion(ChildNode::POSX_POSY_POSZ);

				// Add the new entity to this node's entity map, so it'll be moved to the correct child node below
				entities[entityPARAM->name] = entityPARAM;
				entityPARAM->nodeOwner = this;	// Set node owner for the entity

				// Move all the entities from this node, into the child nodes
				std::map<std::wstring, OctTreeEntity*>::iterator it = entities.begin();
				while (it != entities.end())
				{
					// Determine which child node the entity fits in, regardless of whether the child node exists or not
					ChildNode childNode = ChildNode::NONE;
					if (aabbNNN.getPointIsInside(it->second->position))
						childNode = ChildNode::NEGX_NEGY_NEGZ;
					else if (aabbNNP.getPointIsInside(it->second->position))
						childNode = ChildNode::NEGX_NEGY_POSZ;
					else if (aabbNPN.getPointIsInside(it->second->position))
						childNode = ChildNode::NEGX_POSY_NEGZ;
					else if (aabbNPP.getPointIsInside(it->second->position))
						childNode = ChildNode::NEGX_POSY_POSZ;
					else if (aabbPNN.getPointIsInside(it->second->position))
						childNode = ChildNode::POSX_NEGY_NEGZ;
					else if (aabbPNP.getPointIsInside(it->second->position))
						childNode = ChildNode::POSX_NEGY_POSZ;
					else if (aabbPPN.getPointIsInside(it->second->position))
						childNode = ChildNode::POSX_POSY_NEGZ;
					else if (aabbPPP.getPointIsInside(it->second->position))
						childNode = ChildNode::POSX_POSY_POSZ;

					// Error checking, making sure the entity could fit in one of the eight possible children
					ErrorIfTrue(ChildNode::NONE == childNode, L"OctTreeNode::addEntity() failed when trying to add entity " + it->second->name + L" to any of the eight child nodes as it's position doesn't fit inside any of them.");

					// Create the child node if it doesn't exist
					if (!childNodes[childNode])
					{
						childNodes[childNode] = new OctTreeNode(computeChildNodeRegion(childNode), this, octTree);
						ErrorIfFalse(childNodes[childNode], L"OctTreeNode::addEntity() failed when trying to add entity " + it->second->name + L" to any of the eight child nodes as it was unable to allocate memory for the child node.");
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
		// Compute potential child node regions
		AABB aabbNNN = computeChildNodeRegion(ChildNode::NEGX_NEGY_NEGZ);
		AABB aabbNNP = computeChildNodeRegion(ChildNode::NEGX_NEGY_POSZ);
		AABB aabbNPN = computeChildNodeRegion(ChildNode::NEGX_POSY_NEGZ);
		AABB aabbNPP = computeChildNodeRegion(ChildNode::NEGX_POSY_POSZ);
		AABB aabbPNN = computeChildNodeRegion(ChildNode::POSX_NEGY_NEGZ);
		AABB aabbPNP = computeChildNodeRegion(ChildNode::POSX_NEGY_POSZ);
		AABB aabbPPN = computeChildNodeRegion(ChildNode::POSX_POSY_NEGZ);
		AABB aabbPPP = computeChildNodeRegion(ChildNode::POSX_POSY_POSZ);

		ChildNode childNode = ChildNode::NONE;
		if (aabbNNN.getPointIsInside(entityPARAM->position))
			childNode = ChildNode::NEGX_NEGY_NEGZ;
		else if (aabbNNP.getPointIsInside(entityPARAM->position))
			childNode = ChildNode::NEGX_NEGY_POSZ;
		else if (aabbNPN.getPointIsInside(entityPARAM->position))
			childNode = ChildNode::NEGX_POSY_NEGZ;
		else if (aabbNPP.getPointIsInside(entityPARAM->position))
			childNode = ChildNode::NEGX_POSY_POSZ;
		else if (aabbPNN.getPointIsInside(entityPARAM->position))
			childNode = ChildNode::POSX_NEGY_NEGZ;
		else if (aabbPNP.getPointIsInside(entityPARAM->position))
			childNode = ChildNode::POSX_NEGY_POSZ;
		else if (aabbPPN.getPointIsInside(entityPARAM->position))
			childNode = ChildNode::POSX_POSY_NEGZ;
		else if (aabbPPP.getPointIsInside(entityPARAM->position))
			childNode = ChildNode::POSX_POSY_POSZ;

		// Error checking, making sure the entity could fit in one of the eight possible children
		ErrorIfTrue(ChildNode::NONE == childNode, L"OctTreeNode::addEntity() failed when trying to add entity " + entityPARAM->name + L" to any of the eight child nodes as it's position doesn't fit inside any of them.");

		// Create the child node if it doesn't exist
		if (!childNodes[childNode])
		{
			childNodes[childNode] = new OctTreeNode(computeChildNodeRegion(childNode), this, octTree);
			ErrorIfFalse(childNodes[childNode], L"OctTreeNode::addEntity() failed when trying to add entity " + entityPARAM->name + L" to any of the eight child nodes as it was unable to allocate memory for the child node.");
		}

		// Add the new entity
		childNodes[childNode]->addEntity(entityPARAM);
	}

	void OctTreeNode::removeEntity(OctTreeEntity* entityPARAM)
	{
		// Attempt to find the entity
		std::map<std::wstring, OctTreeEntity*>::iterator it = entities.find(entityPARAM->name);
		ErrorIfTrue(it == entities.end(), L"OctTreeNode::removeEntity() failed. The entity named " + entityPARAM->name + L" could not be found");
		entities.erase(it);
		// No need to delete entity, the OctTree::removeAllEntities() or OctTree::removeEntity() does this
	}

	void OctTreeNode::getNodesWithEntities(std::vector<OctTreeNode*>& nodesPARAM)
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
			for (int i = 0; i < 8; i++)
			{
				if (childNodes[i])
					childNodes[i]->getNodesWithEntities(nodesPARAM);
			}
		}
	}

	void OctTreeNode::getNodesWithEntitiesWhichIntersect(std::vector<OctTreeNode*>& nodesPARAM, const AABB& aabbPARAM)
	{
		// If this node doesn't have any children, just check this node
		if (!hasAnyChildNodes())
		{
			// If this node doesn't have any entities
			if (0 == entities.size())
				return;

			// If the AABB interects with this node, add this node
			if (region.getAABBintersects(aabbPARAM))
			{
				nodesPARAM.push_back(this);
				return;
			}
		}
		else  // This node has children, check those
		{
			for (int i = 0; i < 8; i++)
			{
				if (childNodes[i])
					childNodes[i]->getNodesWithEntitiesWhichIntersect(nodesPARAM, aabbPARAM);
			}
		}
	}

	void OctTreeNode::getNodesWithEntitiesWhichIntersect(std::vector<OctTreeNode*>& nodesPARAM, const Frustum& frustumPARAM)
	{
		// If this node doesn't have any children, just check this node
		if (!hasAnyChildNodes())
		{
			// If this node doesn't have any entities
			if (0 == entities.size())
				return;

			// If the AABB interects with this node, add this node
			if (frustumPARAM.isAABBIntersecting(region))
			{
				nodesPARAM.push_back(this);
				return;
			}
		}
		else  // This node has children, check those
		{
			for (int i = 0; i < 8; i++)
			{
				if (childNodes[i])
					childNodes[i]->getNodesWithEntitiesWhichIntersect(nodesPARAM, frustumPARAM);
			}
		}
	}

	void OctTreeNode::getMaxNodeDepth(unsigned int& maxNodeDepthPARAM)
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