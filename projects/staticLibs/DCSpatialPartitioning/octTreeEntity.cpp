#include "octTreeEntity.h"
#include "octTreeNode.h"

namespace DC
{
	COctTreeEntity::COctTreeEntity(const std::wstring& strName, const Vector3f& vPosition, COctTreeNode* pNodeOwner, int iUserData, void* pUserData)
	{
		_mstrName = strName;
		_mvPosition = vPosition;
		_mpNodeOwner = pNodeOwner;

		// Store user data
		miUserData = iUserData;
		mpUserData = pUserData;
	}

	void COctTreeEntity::debugSetColour(Colour& colour)
	{
		_mColDebug = colour;
	}

	Vector3f COctTreeEntity::getPosition(void)
	{
		return _mvPosition;;
	}

	std::wstring COctTreeEntity::getName(void)
	{
		return _mstrName;
	}
}