#pragma once
#include "../DCMath/vector3f.h"
#include "../DCCommon/colour.h"
#include "../DCCommon/string.h"

namespace DC
{
	class OctTreeNode;

	// An entity which is assigned into an OctTreeNode
	// It contains it's unique name, it's position within the world and the node it belongs to.
	class OctTreeEntity
	{
		friend class OctTree;
		friend class OctTreeNode;
	public:
		// Constructor.
		// name is the unique name given to this entity.
		// position is this entity's position within the world
		OctTreeEntity(const std::wstring& name, const Vector3f& position, OctTreeNode* nodeOwner, int userData = 0, void* pUserData = 0);

		// Set the debug colour of the entity
		void debugSetColour(Colour& colour);

		// Returns position of entity
		Vector3f getPosition(void);

		// Returns name of the entity
		std::wstring getName(void);

		// Below are members which may be set to store various information.
		// They have nothing to do with the oct tree itself.
		int userData;
		void* pUserData;

	private:
		std::wstring name;				// Unique name of this entity
		Vector3f position;				// Position of this entity
		OctTreeNode* nodeOwner;			// The node this entity is in
		Colour debugColour;				// The colour used when debug rendering this entity
	};
}
