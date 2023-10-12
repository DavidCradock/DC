#pragma once
#include "../Math/vector2f.h"
#include "../Common/colour.h"
#include "../Common/string.h"

namespace DC
{
	class QuadTreeNode;

	// An entity which is assigned into a QuadTreeNode
	// It contains it's unique name, it's position within the world and the node it belongs to.
	class QuadTreeEntity
	{
		friend class QuadTree;
		friend class QuadTreeNode;
	public:
		// Constructor.
		// name is the unique name given to this entity.
		// positionX and positionX are this entity's position within the world
		QuadTreeEntity(const std::wstring& name, int positionX, int positionY, QuadTreeNode* nodeOwner, int userData = 0, void *pUserData = 0);

		// Set the debug colour of the entity
		void debugSetColour(Colour& colour);

		// Returns position of entity
		Vector2f getPosition(void);

		// Returns name of the entity
		std::wstring getName(void);

		// Below are members which may be set to store various information.
		// They have nothing to do with the quad tree itself.
		int userData;
		void* pUserData;

	private:
		std::wstring name;			// Unique name of this entity
		int positionX;				// Position of this entity along X axis
		int positionY;				// Position of this entity along Y axis
		QuadTreeNode* nodeOwner;	// The node this entity is in
		Colour debugColour;			// The colour used when debug rendering this entity
	};
}
