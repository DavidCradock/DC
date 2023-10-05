#include "quadTreeEntity.h"
#include "quadTreeNode.h"

namespace DC
{
	CQuadTreeEntity::CQuadTreeEntity(const std::wstring& strName, int iPosX, int iPosY, CQuadTreeNode* pNodeOwner, int iUserData, void* pUserData)
	{
		_mstrName = strName;
		_miPosX = iPosX;
		_miPosY = iPosY;
		_mpNodeOwner = pNodeOwner;

		// Store user data
		miUserData = iUserData;
		mpUserData = pUserData;
	}

	void CQuadTreeEntity::debugSetColour(Colour& colour)
	{
		_mColDebug = colour;
	}

	Vector2f CQuadTreeEntity::getPosition(void)
	{
		Vector2f v;
		v.x = float(_miPosX);
		v.y = float(_miPosY);
		return v;
	}

	std::wstring CQuadTreeEntity::getName(void)
	{
		return _mstrName;
	}
}