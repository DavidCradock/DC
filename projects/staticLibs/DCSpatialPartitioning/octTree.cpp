#include "octTree.h"
#include "../DCCommon/error.h"

namespace DC
{
	OctTree::OctTree(int maxEntitiesPerNodePARAM, float sizeIncreaseMultiplierPARAM)
	{
		_mpRootNode = 0;
		init(maxEntitiesPerNodePARAM, sizeIncreaseMultiplierPARAM);
	}

	void OctTree::init(int maxEntitiesPerNodePARAM, float sizeIncreaseMultiplierPARAM)
	{
		free();

		AABB aabbInitialRootNodeRegion(Vector3f(-8, -8, -8), Vector3f(8, 8, 8));

		// Attempt to create root node, passing 0 as the pointer to the parent node
		_mpRootNode = new OctTreeNode(aabbInitialRootNodeRegion, 0, this);
		ErrorIfFalse(_mpRootNode, L"OctTree::init() failed. Unable to allocate memory for root node.");

		// Make sure valid values were given
		ErrorIfTrue(maxEntitiesPerNodePARAM < 1, L"OctTree::init() failed. Given invalid number for iMaxEntitiesPerNode. Must be at least one.");
		ErrorIfTrue(sizeIncreaseMultiplierPARAM < 2, L"OctTree::init() failed. Given invalid number for fSizeIncreaseMultiplier. Must be at least 2.0f.");

		// Store settings
		_miMaxEntitiesPerNode = maxEntitiesPerNodePARAM;
		_mfSizeIncreaseMultiplier = sizeIncreaseMultiplierPARAM;

		// Current maximum node depth
		_muiCurrentMaxNodeDepth = 0;

		// Compute maximum node depth before node division is forbidden.
		computeMaxNodeDepth();
	}

	OctTree::~OctTree()
	{
		free();
	}

	void OctTree::free(void)
	{
		// Delete root node, which will delete all children and their children and so on.
		// Although this obviously removes the entities from the nodes, because the nodes themselves
		// no longer exist, this does NOT delete the entity pointers. They are stored in this object's
		// _mmapEntities hashmap.
		if (_mpRootNode)
		{
			delete _mpRootNode;
			_mpRootNode = 0;
		}

		// Delete all entities
		std::map<std::wstring, OctTreeEntity*>::iterator it = _mmapEntities.begin();
		while (it != _mmapEntities.end())
		{
			delete it->second;
			it++;
		}
		_mmapEntities.clear();
	}
/*
	void OctTree::debugRender(CSMCamera& camera) const
	{
		// Obtain required resources needed to render the node's as lines.
		CResourceVertexBufferLine* pLine = x->pResource->getVertexBufferLine("X:default");
		CResourceShader* pShader = x->pResource->getShader("X:VBCPT");
		CResourceTexture2DFromFile* pTexture = x->pResource->getTexture2DFromFile("X:default_white");

		CMatrix matrixWorld;

		// Bind shader and set uniforms
		pShader->bind();
		pShader->setMat4("matrixWorld", matrixWorld);
		pShader->setMat4("matrixView", camera.getViewMatrix());
		pShader->setMat4("matrixProjection", camera.getProjectionMatrix());

		// Tell OpenGL, for each sampler, to which texture unit it belongs to
		pShader->setInt("texture0", 0);

		// And bind the texture...
		pTexture->bind();

		// Set the line vertex buffer rendering mode
		pLine->setDrawModeAsLineList();
		pLine->removeGeom();

		glEnable(GL_BLEND);
//		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Call the root node's debugRender method which will render itself and it's children recursively
		_mpRootNode->debugRenderNodes(pLine, Colour(1.0f, 1.0f, 1.0f, 1.0f));

		// Send geometry to be rendered
		pLine->update();
		pLine->render();

		// Cleanup
		glDisable(GL_BLEND);
//		glEnable(GL_DEPTH_TEST);
		pTexture->unbind();
		pShader->unbind();
	}
	*/
	/*
	void OctTree::debugSetEntityColour(const std::wstring& name, Colour& colour)
	{
		// Find the entity
		std::map<std::wstring, OctTreeEntity*>::iterator it = _mmapEntities.find(name);
		ErrorIfTrue(_mmapEntities.end() == it, "OctTree::debugSetEntityColour() failed. The entity name of " + name + " doesn't exist.");
		it->second->_mColDebug = colour;
	}

	void OctTree::debugSetAllEntitiesColour(Colour& colour)
	{
		std::map<std::wstring, OctTreeEntity*>::iterator it = _mmapEntities.begin();
		while (it != _mmapEntities.end())
		{
			it->second->_mColDebug = colour;
			it++;
		}
	}
	*/
	void OctTree::addEntity(const std::wstring& namePARAM, const Vector3f& positionPARAM, int userDataPARAM, void* pUserDataPARAM)
	{
		// Make sure the entity doesn't already exist by checking the hashmap
		std::map<std::wstring, OctTreeEntity*>::iterator it = _mmapEntities.find(namePARAM);
		ErrorIfTrue(_mmapEntities.end() != it, L"OctTree::addEntity() failed. The entity name of " + namePARAM + L" already exists.");

		// Create new entity, setting it's owner to 0
		OctTreeEntity* pEntity = new OctTreeEntity(namePARAM, positionPARAM, 0, userDataPARAM, pUserDataPARAM);
		ErrorIfFalse(pEntity, L"OctTree::addEntity() failed to allocate memory for new entity.");

		// Add entity to hashmap for fast lookup
		_mmapEntities[namePARAM] = pEntity;

		// Determine whether the given position of the entity fits in the root node
		if (!_mpRootNode->_mRegion.getPointIsInside(positionPARAM))
		{
			// The position of the new entity doesn't fit within the root node's area
			// We're going to have to recreate the entire tree

			// Delete root node, which will delete all children and their children and so on.
			// This does not delete the entities (As they're stored in the _mmapEntities hashmap)
			// But before deleting the root node, get it's currently set region which we will multiply to get
			// the root node's new dimensions
			AABB aabbOldRootNodeRegion = _mpRootNode->_mRegion;
			delete _mpRootNode;

			// Inscrease the old root node's region size until the new entity's position fits within
			bool bNewPositionFits = false;
			while (!bNewPositionFits)
			{
				aabbOldRootNodeRegion.resizeArea(_mfSizeIncreaseMultiplier);
				bNewPositionFits = aabbOldRootNodeRegion.getPointIsInside(positionPARAM);
			}

			// Now re-create the root node, passing 0 as the pointer to the parent node and the new region size
			_mpRootNode = new OctTreeNode(aabbOldRootNodeRegion, 0, this);
			ErrorIfFalse(_mpRootNode, L"OctTree::addEntity() failed. Unable to allocate memory for new root node.");

			// Now re-insert all entities stored in the hash map back into the tree
			// The new entity has already been added to the hash map, so no need to insert it seperately
			it = _mmapEntities.begin();
			while (it != _mmapEntities.end())
			{
				_mpRootNode->addEntity(it->second);
				it++;
			}
		}
		else  // The entity position does fit inside the area of this node, simply add it
		{
			_mpRootNode->addEntity(pEntity);
		}
	}

	void OctTree::removeEntity(const std::wstring& namePARAM)
	{
		// Make sure the entity exists by checking the hashmap
		std::map<std::wstring, OctTreeEntity*>::iterator it = _mmapEntities.find(namePARAM);
		ErrorIfTrue(_mmapEntities.end() == it, L"OctTree::removeEntity() failed. The entity name of " + namePARAM + L" doesn't exist.");

		// Get the node the entity is stored in to remove the entity from itself.
		OctTreeNode* pNodeContainingRemovedEntity = it->second->_mpNodeOwner;
		pNodeContainingRemovedEntity->removeEntity(it->second);

		// Remove the entity from the hashmap and delete it
		OctTreeEntity* pEntity = it->second;
		_mmapEntities.erase(it);
		delete pEntity;

		// Now check to see if the node is now empty and if so, remove it
		if (!pNodeContainingRemovedEntity->hasEntitiesInThisAndAllChildren())
		{
			// Is the node which held the removed entity, the root node
			bool bNodeIsRoot = _mpRootNode == pNodeContainingRemovedEntity;

			if (!bNodeIsRoot)  // The node wasn't the root node and therefore pNodeParent will contain a valid pointer to it's parent node.
			{
				// Store pointer to the parent
				OctTreeNode* pNodeParent = pNodeContainingRemovedEntity->_mpParentNode;

				// We're done, except...
				// We need to traverse up to the root node and delete any NOW empty nodes, which may 
				// have only existed to hold the now deleted node.
				// We also have to remove any parent node's _mpChildNode[8] pointers if we delete a node.
				while (pNodeParent)
				{
					// Go through each of the eight possible child nodes of the parent
					for (int i = 0; i < 8; i++)
					{
						if (pNodeParent->_mpChildNode[i])	// There is a child at this array position
						{
							if (!pNodeParent->_mpChildNode[i]->hasEntitiesInThisAndAllChildren())	// Node is empty
							{
								delete pNodeParent->_mpChildNode[i];
								pNodeParent->_mpChildNode[i] = 0;
							}
						}
					}
					pNodeParent = pNodeParent->_mpParentNode;
				}

				// This will delete the node and all it's children.
				// No need to delete, as it's deleted above when deleting children.
				// If we attempted to delete again, all hell breaks loose!
//				delete pNodeContainingRemovedEntity;
			}
		}
		// The node which contained the entity has other entities, leave it alone.
	}

	bool OctTree::getEntityExists(const std::wstring& namePARAM) const
	{
		// Check the hashmap
		std::map<std::wstring, OctTreeEntity*>::iterator it = _mmapEntities.find(namePARAM);
		return(_mmapEntities.end() != it);
	}

	void OctTree::removeAllEntities(bool resetTreePARAM)
	{
		// Go through each entity, asking each node which it's in to remove itself
		std::map<std::wstring, OctTreeEntity*>::iterator it = _mmapEntities.begin();
		while (it != _mmapEntities.end())
		{
			// Get the node the entity is stored in to remove the entity from itself.
			it->second->_mpNodeOwner->removeEntity(it->second);

			// Remove the entity from the hashmap and delete it
			OctTreeEntity* pEntity = it->second;
			_mmapEntities.erase(it);
			delete pEntity;
			it = _mmapEntities.begin();
		}

		// Now all entities are removed and deleted, reset the tree if bResetTree desires it
		if (resetTreePARAM)
		{
			// Get the root node's AABB, so we can re-create it
			AABB aabbRootNode = _mpRootNode->_mRegion;
			delete _mpRootNode;
			_mpRootNode = new OctTreeNode(aabbRootNode, 0, this);

			// Reset current max node depth
			_muiCurrentMaxNodeDepth = 0;

			// Compute new maximum node depth
			computeMaxNodeDepth();
		}
	}

	void OctTree::setEntityPosition(const std::wstring& namePARAM, const Vector3f& positionPARAM)
	{
		// First make sure the named entity exists
		std::map<std::wstring, OctTreeEntity*>::iterator it = _mmapEntities.find(namePARAM);
		ErrorIfTrue(it == _mmapEntities.end(), L"OctTree::setEntityPosition() failed. The named entity of " + namePARAM + L" doesn't exist.");

		// First check to see if the new entity position still fits within it's current node
		// If it does, we simply update the position
		if (it->second->_mpNodeOwner->_mRegion.getPointIsInside(positionPARAM))
		{
			it->second->_mvPosition = positionPARAM;
			return;
		}

		// If we get here, the new position doesn't fit within the entity's current node

		// Remove the entity from the tree and then re-insert it
		int iUserData = it->second->miUserData;
		void* pUserData = it->second->mpUserData;
		removeEntity(namePARAM);
		addEntity(namePARAM, positionPARAM, iUserData, pUserData);
	}

	void OctTree::getEntityPosition(const std::wstring& namePARAM, Vector3f& positionPARAM) const
	{
		// First make sure the named entity exists
		std::map<std::wstring, OctTreeEntity*>::iterator it = _mmapEntities.find(namePARAM);
		ErrorIfTrue(it == _mmapEntities.end(), L"OctTree::getEntityPosition() failed. The named entity of " + namePARAM + L" doesn't exist.");
		positionPARAM = it->second->_mvPosition;
	}

	void OctTree::computeMaxNodeDepth(void)
	{
		// Obtain smallest dimension of root node
		Vector3f vRootNodeDims = _mpRootNode->_mRegion.getDimensions();
		float fSmallestDim = vRootNodeDims.x;
		if (vRootNodeDims.y < fSmallestDim)
			fSmallestDim = vRootNodeDims.y;
		if (vRootNodeDims.z < fSmallestDim)
			fSmallestDim = vRootNodeDims.z;

		// Now compute max node depth
		_muiMaxNodeDepth = 0;
		while (fSmallestDim > 2)
		{
			fSmallestDim *= 0.5f;
			_muiMaxNodeDepth++;
		}
	}

	std::vector<OctTreeNode*> OctTree::getNodesWithEntities(void) const
	{
		std::vector<OctTreeNode*> vResult;
		_mpRootNode->getNodesWithEntities(vResult);
		return vResult;
	}

	std::vector<OctTreeNode*> OctTree::getNodesWithEntitiesWhichIntersect(const AABB& aabbPARAM) const
	{
		std::vector<OctTreeNode*> vResult;
		_mpRootNode->getNodesWithEntitiesWhichIntersect(vResult, aabbPARAM);
		return vResult;
	}

	std::vector<OctTreeNode*> OctTree::getNodesWithEntitiesWhichIntersect(const Frustum& frustumPARAM) const
	{
		std::vector<OctTreeNode*> vResult;
		_mpRootNode->getNodesWithEntitiesWhichIntersect(vResult, frustumPARAM);
		return vResult;
	}

	std::vector<OctTreeEntity*> OctTree::getEntitiesWithinRange(const Vector3f& positionPARAM, float rangePARAM) const
	{
		// Create an AABB which covers the maximum range from the given position
		AABB aabb;
		float fRangeTimesTwo = rangePARAM * 2.0f;
		aabb.setPosDims(positionPARAM, Vector3f(fRangeTimesTwo, fRangeTimesTwo, fRangeTimesTwo));

		// Go through the nodes and get a vector of nodes which intersect the AABB and have entities in them
		std::vector<OctTreeNode*> vNodes = getNodesWithEntitiesWhichIntersect(aabb);
		std::vector<OctTreeEntity*> vResult;
		for (unsigned int ui = 0; ui < vNodes.size(); ui++)
		{
			// Go through each entity within each node and add them to the resulting vector
			std::map<std::wstring, OctTreeEntity*>::iterator it = vNodes[ui]->_mmapEntities.begin();
			while (it != vNodes[ui]->_mmapEntities.end())
			{
				vResult.push_back(it->second);
				it++;
			}
		}
		return vResult;
	}

	std::vector<OctTreeEntity*> OctTree::getEntitiesWithinAABB(const AABB& aabbPARAM) const
	{
		// Go through the nodes and get a vector of the nodes which intersect the AABB and have entities in them
		std::vector<OctTreeNode*> vNodes = getNodesWithEntitiesWhichIntersect(aabbPARAM);
		std::vector<OctTreeEntity*> vResult;
		for (unsigned int ui = 0; ui < vNodes.size(); ui++)
		{
			// Go through each entity within each node and add them to the resulting vector
			std::map<std::wstring, OctTreeEntity*>::iterator it = vNodes[ui]->_mmapEntities.begin();
			while (it != vNodes[ui]->_mmapEntities.end())
			{
				vResult.push_back(it->second);
				it++;
			}
		}
		return vResult;
	}

	std::vector<OctTreeEntity*> OctTree::getEntitiesWithinFrustum(const Frustum& frustumPARAM) const
	{
		// Go through the nodes and get a vector of the nodes which intersect the frustum and have entities in them
		std::vector<OctTreeNode*> vNodes = getNodesWithEntitiesWhichIntersect(frustumPARAM);
		std::vector<OctTreeEntity*> vResult;
		for (unsigned int ui = 0; ui < vNodes.size(); ui++)
		{
			// Go through each entity within each node and add them to the resulting vector
			std::map<std::wstring, OctTreeEntity*>::iterator it = vNodes[ui]->_mmapEntities.begin();
			while (it != vNodes[ui]->_mmapEntities.end())
			{
				vResult.push_back(it->second);
				it++;
			}
		}
		return vResult;
	}

	unsigned int OctTree::getNodeDepthCurrent(void)
	{
		// We have to recompute this, so go through all nodes, get their depth and compare
		_muiCurrentMaxNodeDepth = 0;
		_mpRootNode->getMaxNodeDepth(_muiCurrentMaxNodeDepth);
		return _muiCurrentMaxNodeDepth;
	}

	unsigned int OctTree::getNodeDepthMax(void) const
	{
		return _muiMaxNodeDepth;
	}

}