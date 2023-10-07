#include "octTreeEntity.h"
#include "octTreeNode.h"

namespace DC
{
	OctTreeEntity::OctTreeEntity(const std::wstring& namePARAM, const Vector3f& positionPARAM, OctTreeNode* nodeOwnerPARAM, int userDataPARAM, void* pUserDataPARAM)
	{
		_mstrName = namePARAM;
		_mvPosition = positionPARAM;
		_mpNodeOwner = nodeOwnerPARAM;

		// Store user data
		miUserData = userDataPARAM;
		mpUserData = pUserDataPARAM;
	}

	void OctTreeEntity::debugSetColour(Colour& colourPARAM)
	{
		_mColDebug = colourPARAM;
	}

	Vector3f OctTreeEntity::getPosition(void)
	{
		return _mvPosition;;
	}

	std::wstring OctTreeEntity::getName(void)
	{
		return _mstrName;
	}
}