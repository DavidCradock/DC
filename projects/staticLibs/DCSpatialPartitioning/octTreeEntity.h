#pragma once
#include "../DCMath/vector3f.h"
#include "../DCCommon/colour.h"
#include "../DCCommon/string.h"

namespace DC
{
	class COctTreeNode;

	// An entity which is assigned into an COctTreeNode
	// It contains it's unique name, it's position within the world and the node it belongs to.
	class COctTreeEntity
	{
		friend class COctTree;
		friend class COctTreeNode;
	public:
		// Constructor.
		// strName is the unique name given to this entity.
		// vPosition is this entity's position within the world
		COctTreeEntity(const std::wstring& strName, const Vector3f& vPosition, COctTreeNode* pNodeOwner, int iUserData = 0, void* pUserData = 0);

		// Set the debug colour of the entity
		void debugSetColour(Colour& colour);

		// Returns position of entity
		Vector3f getPosition(void);

		// Returns name of the entity
		std::wstring getName(void);

		// Below are members which may be set to store various information.
		// They have nothing to do with the oct tree itself.
		int miUserData;
		void* mpUserData;

	private:
		std::wstring _mstrName;			// Unique name of this entity
		Vector3f _mvPosition;			// Position of this entity
		COctTreeNode* _mpNodeOwner;		// The node this entity is in
		Colour _mColDebug;				// The colour used when debug rendering this entity
	};
}
