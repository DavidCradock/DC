#include "quadTreeEntity.h"
#include "quadTreeNode.h"

namespace DC
{
	QuadTreeEntity::QuadTreeEntity(const std::wstring& namePARAM, int positionXPARAM, int positionYPARAM, QuadTreeNode* nodeOwnerPARAM, int userDataPARAM, void* pUserDataPARAM)
	{
		name = namePARAM;
		positionX = positionXPARAM;
		positionY = positionYPARAM;
		nodeOwner = nodeOwnerPARAM;

		// Store user data
		userData = userDataPARAM;
		pUserData = pUserDataPARAM;
	}

	void QuadTreeEntity::debugSetColour(Colour& colourPARAM)
	{
		debugColour = colourPARAM;
	}

	Vector2f QuadTreeEntity::getPosition(void)
	{
		Vector2f v;
		v.x = float(positionX);
		v.y = float(positionY);
		return v;
	}

	std::wstring QuadTreeEntity::getName(void)
	{
		return name;
	}
}