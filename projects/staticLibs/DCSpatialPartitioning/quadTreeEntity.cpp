#include "quadTreeEntity.h"
#include "quadTreeNode.h"

namespace DC
{
	QuadTreeEntity::QuadTreeEntity(const std::wstring& namePARAM, int positionXPARAM, int positionYPARAM, QuadTreeNode* nodeOwnerPARAM, int userDataPARAM, void* pUserDataPARAM)
	{
		_mstrName = namePARAM;
		_miPosX = positionXPARAM;
		_miPosY = positionYPARAM;
		_mpNodeOwner = nodeOwnerPARAM;

		// Store user data
		miUserData = userDataPARAM;
		mpUserData = pUserDataPARAM;
	}

	void QuadTreeEntity::debugSetColour(Colour& colourPARAM)
	{
		_mColDebug = colourPARAM;
	}

	Vector2f QuadTreeEntity::getPosition(void)
	{
		Vector2f v;
		v.x = float(_miPosX);
		v.y = float(_miPosY);
		return v;
	}

	std::wstring QuadTreeEntity::getName(void)
	{
		return _mstrName;
	}
}