#include "quadTree.h"
#include "../DCCommon/error.h"

namespace DC
{
	QuadTree::QuadTree(int maxEntitiesPerNodePARAM, int rectSizeIncreaseMultiplierPARAM)
	{
		rootNode = 0;

		init(maxEntitiesPerNodePARAM, rectSizeIncreaseMultiplierPARAM);
	}

	void QuadTree::init(int maxEntitiesPerNodePARAM, int rectSizeIncreaseMultiplierPARAM)
	{
		free();

		Rect rctInitialRootNodeRegion(-1024, -1024, 1024, 1024);

		// Attempt to create root node, passing 0 as the pointer to the parent node
		rootNode = new QuadTreeNode(rctInitialRootNodeRegion, 0, this);
		ErrorIfFalse(rootNode, L"QuadTree::init() failed. Unable to allocate memory for root node.");

		// Make sure valid values were given
		ErrorIfTrue(maxEntitiesPerNodePARAM < 1, L"QuadTree::init() failed. Given invalid number for iMaxEntitiesPerNode. Must be at least one.");
		ErrorIfTrue(rectSizeIncreaseMultiplierPARAM < 2, L"QuadTree::init() failed. Given invalid number for iRectSizeIncreaseMultiplier. Must be at least 2.");

		// Store settings
		maxEntitiesPerNode = maxEntitiesPerNodePARAM;
		rectSizeIncreaseMultiplier = rectSizeIncreaseMultiplierPARAM;

		// Current maximum node depth
		currentMaxNodeDepth = 0;

		// Compute maximum node depth before node division is forbidden.
		computeMaxNodeDepth();
	}

	QuadTree::~QuadTree()
	{
		free();
	}

	void QuadTree::free(void)
	{
		// Delete root node, which will delete all children and their children and so on.
		// Although this obviously removes the entities from the nodes, because the nodes themselves
		// no longer exist, this does NOT delete the entity pointers. They are stored in this object's
		// _mmapEntities hashmap.
		if (rootNode)
		{
			delete rootNode;
			rootNode = 0;
		}

		// Delete all entities
		std::map<std::wstring, QuadTreeEntity*>::iterator it = entities.begin();
		while (it != entities.end())
		{
			delete it->second;
			it++;
		}
		entities.clear();
	}
/*
	void QuadTree::debugRender(const Vector3f& cameraPosition, bool renderNodes, bool renderEntities, int entityCircleRadius, unsigned int entityCircleNumSegments) const
	{
		if (!bRenderNodes && !bRenderEntities)
			return;

		// Obtain required resources needed to render the node's as lines.
		CResourceVertexBufferLine* pLine = x->pResource->getVertexBufferLine("X:default");
		CResourceShader* pShader = x->pResource->getShader("X:VBCPT");
		CResourceTexture2DFromFile* pTexture = x->pResource->getTexture2DFromFile("X:default_white");

		// Setup orthographic projection matrix
		CMatrix matrixWorld;
		CMatrix matrixView;
		Vector3f vTargetPos = vCameraPosition;
		vTargetPos.z -= 100.0f;
		matrixView.setViewLookat(vCameraPosition, vTargetPos, Vector3f(0.0f, 1.0f, 0.0f));

		CMatrix matrixProjection;
		matrixProjection.setProjectionPerspective(55.0f, 1.0f, 10000.0f);

		// Bind shader and set uniforms
		pShader->bind();
		pShader->setMat4("matrixWorld", matrixWorld);
		pShader->setMat4("matrixView", matrixView);
		pShader->setMat4("matrixProjection", matrixProjection);

		// Tell OpenGL, for each sampler, to which texture unit it belongs to
		pShader->setInt("texture0", 0);

		// And bind the texture...
		pTexture->bind();

		// Set the line vertex buffer rendering mode
		pLine->setDrawModeAsLineList();
		pLine->removeGeom();

		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Setup vertex we'll be using to render with
		CResourceVertexBufferLine::Vertex vertex;
		vertex.colour.set(1.0f, 1.0f, 1.0f, 1.0f);

		// Call the root node's debugRender method which will render itself and it's children recursively
		if (bRenderNodes)
			_mpRootNode->debugRenderNodes(pLine, vertex, Colour(1.0f, 1.0f, 1.0f, 1.0f));
		if (bRenderEntities)
			_mpRootNode->debugRenderEntities(pLine, iEntityCircleRadius, uiEntityCircleNumSegments);

		// Send geometry to be rendered
		pLine->update();
		pLine->render();

		// Cleanup
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		pTexture->unbind();
		pShader->unbind();
	}
*/
	void QuadTree::debugSetEntityColour(const std::wstring& namePARAM, Colour& colourPARAM)
	{
		// Find the entity
		std::map<std::wstring, QuadTreeEntity*>::iterator it = entities.find(namePARAM);
		ErrorIfTrue(entities.end() == it, L"QuadTree::debugSetEntityColour() failed. The entity name of " + namePARAM + L" doesn't exist.");
		it->second->debugColour = colourPARAM;
	}

	void QuadTree::debugSetAllEntitiesColour(Colour& colourPARAM)
	{
		std::map<std::wstring, QuadTreeEntity*>::iterator it = entities.begin();
		while (it != entities.end())
		{
			it->second->debugColour = colourPARAM;
			it++;
		}
	}

	void QuadTree::addEntity(const std::wstring& namePARAM, int positionXPARAM, int positionYPARAM, int userDataPARAM, void* pUserDataPARAM)
	{
		// Make sure the entity doesn't already exist by checking the hashmap
		std::map<std::wstring, QuadTreeEntity*>::iterator it = entities.find(namePARAM);
		ErrorIfTrue(entities.end() != it, L"QuadTree::addEntity() failed. The entity name of " + namePARAM + L" already exists.");

		// Create new entity, setting it's owner to 0
		QuadTreeEntity* pEntity = new QuadTreeEntity(namePARAM, positionXPARAM, positionYPARAM, 0, userDataPARAM, pUserDataPARAM);
		ErrorIfFalse(pEntity, L"QuadTree::addEntity() failed to allocate memory for new entity.");

		// Add entity to hashmap for fast lookup
		entities[namePARAM] = pEntity;

		// Determine whether the given position of the entity fits in the root node
		if (!rootNode->rectRegion.doesPositionFitWithin(positionXPARAM, positionYPARAM))
		{
			// The position of the new entity doesn't fit within the root node's area
			// We're going to have to recreate the entire tree

			// Delete root node, which will delete all children and their children and so on.
			// This does not delete the entities (As they're stored in the _mmapEntities hashmap)
			// But before deleting the root node, get it's currently set region which we will multiply to get
			// the root node's new dimensions
			Rect rectOldRootNodeRegion = rootNode->rectRegion;
			delete rootNode;

			// Inscrease the old root node's region size until the new entity's position fits within
			bool bNewPositionFits = false;
			while (!bNewPositionFits)
			{
				rectOldRootNodeRegion.resizeArea(rectSizeIncreaseMultiplier);
				bNewPositionFits = rectOldRootNodeRegion.doesPositionFitWithin(positionXPARAM, positionYPARAM);
			}

			// Now re-create the root node, passing 0 as the pointer to the parent node and the new region size
			rootNode = new QuadTreeNode(rectOldRootNodeRegion, 0, this);
			ErrorIfFalse(rootNode, L"QuadTree::addEntity() failed. Unable to allocate memory for new root node.");

			// Now re-insert all entities stored in the hash map back into the tree
			// The new entity has already been added to the hash map, so no need to insert it seperately
			it = entities.begin();
			while (it != entities.end())
			{
				rootNode->addEntity(it->second);
				it++;
			}
		}
		else  // The entity position does fit inside the rect of this node, simply add it
		{
			rootNode->addEntity(pEntity);
		}
	}

	void QuadTree::removeEntity(const std::wstring& namePARAM)
	{
		// Make sure the entity exists by checking the hashmap
		std::map<std::wstring, QuadTreeEntity*>::iterator it = entities.find(namePARAM);
		ErrorIfTrue(entities.end() == it, L"QuadTree::removeEntity() failed. The entity name of " + namePARAM + L" doesn't exist.");

		// Get the node the entity is stored in to remove the entity from itself.
		QuadTreeNode* pNodeContainingRemovedEntity = it->second->nodeOwner;
		pNodeContainingRemovedEntity->removeEntity(it->second);

		// Remove the entity from the hashmap and delete it
		QuadTreeEntity* pEntity = it->second;
		entities.erase(it);
		delete pEntity;

		// Now check to see if the node is now empty and if so, remove it
		if (!pNodeContainingRemovedEntity->hasEntitiesInThisAndAllChildren())
		{
			// Is the node which held the removed entity, the root node
			bool bNodeIsRoot = rootNode == pNodeContainingRemovedEntity;

			if (!bNodeIsRoot)  // The node wasn't the root node and therefore pNodeParent will contain a valid pointer to it's parent node.
			{
				// Store pointer to the parent
				QuadTreeNode* pNodeParent = pNodeContainingRemovedEntity->parentNode;

				// We're done, except...
				// We need to traverse up to the root node and delete any NOW empty nodes, which may 
				// have only existed to hold the now deleted node.
				// We also have to remove any parent node's _mpChildNode[4] pointers if we delete a node.
				while (pNodeParent)
				{
					// Go through each of the four possible child nodes of the parent
					for (int i = 0; i < 4; i++)
					{
						if (pNodeParent->childNodes[i])	// There is a child at this array position
						{
							if (!pNodeParent->childNodes[i]->hasEntitiesInThisAndAllChildren())	// Node is empty
							{
								delete pNodeParent->childNodes[i];
								pNodeParent->childNodes[i] = 0;
							}
						}
					}
					pNodeParent = pNodeParent->parentNode;
				}

				// This will delete the node and all it's children.
				// No need to delete, as it's deleted above when deleting children.
				// If we attempted to delete again, all hell breaks loose!
//				delete pNodeContainingRemovedEntity;
			}
		}	
		// The node which contained the entity has other entities, leave it alone.
	}

	bool QuadTree::getEntityExists(const std::wstring& namePARAM) const
	{
		// Check the hashmap
		std::map<std::wstring, QuadTreeEntity*>::iterator it = entities.find(namePARAM);
		return(entities.end() != it);
	}

	void QuadTree::removeAllEntities(bool resetTreePARAM)
	{
		// Go through each entity, asking each node which it's in to remove itself
		std::map<std::wstring, QuadTreeEntity*>::iterator it = entities.begin();
		while (it != entities.end())
		{
			// Get the node the entity is stored in to remove the entity from itself.
			it->second->nodeOwner->removeEntity(it->second);

			// Remove the entity from the hashmap and delete it
			QuadTreeEntity* pEntity = it->second;
			entities.erase(it);
			delete pEntity;
			it = entities.begin();
		}

		// Now all entities are removed and deleted, reset the tree if bResetTree desires it
		if (resetTreePARAM)
		{
			// Get the root node's rect, so we can re-create it
			Rect rectRootNode = rootNode->rectRegion;
			delete rootNode;
			rootNode = new QuadTreeNode(rectRootNode, 0, this);

			// Reset current max node depth
			currentMaxNodeDepth = 0;

			// Compute new maximum node depth
			computeMaxNodeDepth();
		}
	}

	void QuadTree::setEntityPosition(const std::wstring& namePARAM, int positionXPARAM, int positionYPARAM)
	{
		// First make sure the named entity exists
		std::map<std::wstring, QuadTreeEntity*>::iterator it = entities.find(namePARAM);
		ErrorIfTrue(it == entities.end(), L"QuadTree::setEntityPosition() failed. The named entity of " + namePARAM + L" doesn't exist.");

		// First check to see if the new entity position still fits within it's current node
		// If it does, we simply update the position
		if (it->second->nodeOwner->rectRegion.doesPositionFitWithin(positionXPARAM, positionYPARAM))
		{
			it->second->positionX = positionXPARAM;
			it->second->positionY = positionYPARAM;
			return;
		}

		// If we get here, the new position doesn't fit within the entity's current node

		// Remove the entity from the tree and then re-insert it
		int iUserData = it->second->userData;
		void* pUserData = it->second->pUserData;
		removeEntity(namePARAM);
		addEntity(namePARAM, positionXPARAM, positionYPARAM, iUserData, pUserData);
	}

	void QuadTree::getEntityPosition(const std::wstring& namePARAM, int& positionXPARAM, int& positionYPARAM) const
	{
		// First make sure the named entity exists
		std::map<std::wstring, QuadTreeEntity*>::iterator it = entities.find(namePARAM);
		ErrorIfTrue(it == entities.end(), L"QuadTree::getEntityPosition() failed. The named entity of " + namePARAM + L" doesn't exist.");
		positionXPARAM = it->second->positionX;
		positionYPARAM = it->second->positionY;
	}

	void QuadTree::computeMaxNodeDepth(void)
	{
		// Obtain smallest dimension of root node
		int iDimOfRootX = rootNode->rectRegion.maxX - rootNode->rectRegion.minX;
		int iDimOfRootY = rootNode->rectRegion.maxY - rootNode->rectRegion.minY;
		int iSmallestDim = iDimOfRootX;
		if (iDimOfRootY < iDimOfRootX)
			iSmallestDim = iDimOfRootY;
		// Now compute max node depth
		maxNodeDepth = 0;
		while (iSmallestDim > 2)
		{
			iSmallestDim /= 2;
			maxNodeDepth++;
		}
	}

	std::vector<QuadTreeNode*> QuadTree::getNodesWithEntities(void) const
	{
		std::vector<QuadTreeNode*> vResult;
		rootNode->getNodesWithEntities(vResult);
		return vResult;
	}

	std::vector<QuadTreeNode*> QuadTree::getNodesWithEntitiesWhichIntersect(const Rect& rectPARAM) const
	{
		std::vector<QuadTreeNode*> vResult;
		rootNode->getNodesWithEntitiesWhichIntersect(vResult, rectPARAM);
		return vResult;
	}

	std::vector<QuadTreeEntity*> QuadTree::getEntitiesWithinRange(int positionXPARAM, int positionYPARAM, int rangePARAM) const
	{
		// Create a rect which covers the maximum range from the given position
		Rect rectRange;
		rectRange.minX = positionXPARAM - rangePARAM;
		rectRange.maxX = positionXPARAM + rangePARAM;
		rectRange.minY = positionYPARAM - rangePARAM;
		rectRange.maxY = positionYPARAM + rangePARAM;

		// Go through the nodes and get a vector of nodes which intersect the rect and have entities in them
		std::vector<QuadTreeNode*> vNodes = getNodesWithEntitiesWhichIntersect(rectRange);
		std::vector<QuadTreeEntity*> vResult;
		for (unsigned int ui = 0; ui < vNodes.size(); ui++)
		{
			// Go through each entity within each node and add them to the resulting vector
			std::map<std::wstring, QuadTreeEntity*>::iterator it = vNodes[ui]->entities.begin();
			while (it != vNodes[ui]->entities.end())
			{
				vResult.push_back(it->second);
				it++;
			}
		}
		return vResult;
	}

	std::vector<QuadTreeEntity*> QuadTree::getEntitiesWithinRect(const Rect& rectPARAM) const
	{
		// Go through the nodes and get a vector of the nodes which intersect the rect and have entities in them
		std::vector<QuadTreeNode*> vNodes = getNodesWithEntitiesWhichIntersect(rectPARAM);
		std::vector<QuadTreeEntity*> vResult;
		for (unsigned int ui = 0; ui < vNodes.size(); ui++)
		{
			// Go through each entity within each node and add them to the resulting vector
			std::map<std::wstring, QuadTreeEntity*>::iterator it = vNodes[ui]->entities.begin();
			while (it != vNodes[ui]->entities.end())
			{
				vResult.push_back(it->second);
				it++;
			}
		}
		return vResult;
	}

	unsigned int QuadTree::getNodeDepthCurrent(void)
	{
		// We have to recompute this, so go through all nodes, get their depth and compare
		currentMaxNodeDepth = 0;
		rootNode->getMaxNodeDepth(currentMaxNodeDepth);
		return currentMaxNodeDepth;
	}

	unsigned int QuadTree::getNodeDepthMax(void) const
	{
		return maxNodeDepth;
	}

}