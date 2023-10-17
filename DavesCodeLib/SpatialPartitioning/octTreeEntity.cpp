#include "octTreeEntity.h"
#include "octTreeNode.h"

namespace DC
{
	OctTreeEntity::OctTreeEntity(const std::wstring& namePARAM, const Vector3f& positionPARAM, OctTreeNode* nodeOwnerPARAM, int userDataPARAM, void* pUserDataPARAM)
	{
		name = namePARAM;
		position = positionPARAM;
		nodeOwner = nodeOwnerPARAM;

		// Store user data
		userData = userDataPARAM;
		pUserData = pUserDataPARAM;
	}

	void OctTreeEntity::debugSetColour(Colour& colourPARAM)
	{
		debugColour = colourPARAM;
	}

	Vector3f OctTreeEntity::getPosition(void)
	{
		return position;;
	}

	std::wstring OctTreeEntity::getName(void)
	{
		return name;
	}
}