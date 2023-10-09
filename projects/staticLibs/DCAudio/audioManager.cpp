#include "audioManager.h"
#include "../DCCommon/error.h"

namespace DC
{
	SCAudioManager::SCAudioManager()
	{
		HRESULT hr;
		hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		ErrorIfTrue(FAILED(hr), L"AudioManager() failed to initialise COM.");

		_mpXAudio2 = nullptr;
		ErrorIfTrue(FAILED(hr = XAudio2Create(&_mpXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)), L"AudioManager() failed to create instance of the XAudio2 engine.");

		_mpMasterVoice = nullptr;
		ErrorIfTrue(FAILED(hr = _mpXAudio2->CreateMasteringVoice(&_mpMasterVoice)), L"AudioManager() failed to create mastering voice.");

		addNewSampleGroup(L"default");
	}

	unsigned int SCAudioManager::getNumSampleGroups(void)
	{
		return (unsigned int)_mmapGroup.size();
	}

	unsigned int SCAudioManager::getNumSamplesInGroup(const String& strGroupName)
	{
		if (!sampleGroupExists(strGroupName))
		{
			String err(L"AudioManager::getNumSamplesInGroup(\"");
			err.append(strGroupName);
			err.append(L"\") failed. The group doesn't exist.");
			ErrorIfTrue(1, err);
		}
		std::map<String, Group*>::iterator itg = _mmapGroup.find(strGroupName);
		return (unsigned int)itg->second->mmapResource.size();;
	}

	unsigned int SCAudioManager::getNumSamplesInGroupLoaded(const String& strGroupName)
	{
		if (!sampleGroupExists(strGroupName))
		{
			String err(L"AudioManager::getNumSamplesInGroupLoaded(\"");
			err.append(strGroupName);
			err.append(L"\") failed. The group doesn't exist.");
			ErrorIfTrue(1, err);
		}
		std::map<String, Group*>::iterator itg = _mmapGroup.find(strGroupName);

		unsigned int iResLoadedTotal = 0;
		std::map<String, ResourceSample*>::iterator it = itg->second->mmapResource.begin();
		while (it != itg->second->mmapResource.end())
		{
			if (it->second->bLoaded)
				++iResLoadedTotal;
			it++;
		}
		return iResLoadedTotal;
	}

	const String& SCAudioManager::getSampleGroupName(unsigned int iGroupIndex)
	{
		if (iGroupIndex >= _mmapGroup.size())
		{
			String err(L"AudioManager::getSampleGroupName(");
			
			err.appendUnsignedInt(iGroupIndex);
			err.append(L") failed. Invalid index given. Number of groups is ");
			err.appendUnsignedInt(getNumSampleGroups());
			ErrorIfTrue(1, err);
		}
		std::map<String, Group*>::iterator itg = _mmapGroup.begin();
		unsigned int i = 0;
		while (i < iGroupIndex)
		{
			itg++;
			++i;
		}
		return itg->first;
	}

	void SCAudioManager::addNewSampleGroup(const String& strNewGroupName)
	{
		if (sampleGroupExists(strNewGroupName))
		{
			return;
		}
		Group* pNewGroup = new Group;
		_mmapGroup[strNewGroupName] = pNewGroup;
	}

	bool SCAudioManager::sampleGroupExists(const String& strGroupName)
	{
		std::map<String, Group*>::iterator it = _mmapGroup.find(strGroupName);
		if (it == _mmapGroup.end())
			return false;
		return true;
	}

	void SCAudioManager::loadSampleGroup(const String& strGroupName)
	{
		// Group doesn't exist?
		if (!sampleGroupExists(strGroupName))
		{
			String err(L"AudioManager::loadSampleGroup(\"");
			err.append(strGroupName);
			err.append(L"\") failed. As the given named group doesn't exist.");
			ErrorIfTrue(1, err);
		}

		// Load any unloaded resources within the group
		std::map<String, Group*>::iterator itg = _mmapGroup.find(strGroupName);

		// For each resource in this group...
		std::map<String, ResourceSample*>::iterator it = itg->second->mmapResource.begin();
		while (it != itg->second->mmapResource.end())
		{
			if (!it->second->bLoaded)
			{
				it->second->pResource->load();
				it->second->bLoaded = true;
			}
			it++;
		}
	}

	void SCAudioManager::loadSampleGroupSingle(const String& strGroupName)
	{
		// Group doesn't exist?
		if (!sampleGroupExists(strGroupName))
		{
			String err(L"AudioManager::loadSampleGroupSingle(\"");
			err.append(strGroupName);
			err += L"\") failed. As the given named group doesn't exist.";
			ErrorIfTrue(1, err);
		}

		// Load any unloaded resources within the group
		std::map<String, Group*>::iterator itg = _mmapGroup.find(strGroupName);

		// For each resource in this group...
		std::map<String, ResourceSample*>::iterator it = itg->second->mmapResource.begin();
		while (it != itg->second->mmapResource.end())
		{
			if (!it->second->bLoaded)
			{
				it->second->pResource->load();
				it->second->bLoaded = true;
				return;	// We've changed a resource from unloaded to loaded state, aka, we've loaded a resource, simply return.
			}
			it++;
		}
	}

	void SCAudioManager::unloadSampleGroup(const String& strGroupName)
	{
		// Group doesn't exist?
		if (!sampleGroupExists(strGroupName))
		{
			String err(L"AudioManager::unloadSampleGroup(\"");
			err.append(strGroupName);
			err += L"\") failed. As the given named group doesn't exist.";
			ErrorIfTrue(1, err);
		}

		// Unload any loaded resources within the group
		std::map<String, Group*>::iterator itg = _mmapGroup.find(strGroupName);

		// For each resource in this group...
		std::map<String, ResourceSample*>::iterator it = itg->second->mmapResource.begin();
		while (it != itg->second->mmapResource.end())
		{
			if (it->second->bLoaded)
			{
				it->second->pResource->unload();
				it->second->bLoaded = false;
			}
			it++;
		}
	}

	CAudioSample* SCAudioManager::addSample(const String& strNewResourceName, const String& strGroupName)
	{
		// Group doesn't exist?
		if (!sampleGroupExists(strGroupName))
		{
			String err(L"AudioManager::addSample(\"");
			err.append(strNewResourceName);
			err += L"\", \"";
			err.append(strGroupName);
			err += L"\") failed. As the given named group of \"";
			err.append(strGroupName);
			err += L"\" which the new resource was to be placed into, doesn't exist.";
			ErrorIfTrue(1, err);
		}

		// Resource already exists in the group?
		std::map<String, Group*>::iterator itg = _mmapGroup.find(strGroupName);									// Get iterator to the group (we know it exists)
		std::map<String, ResourceSample*>::iterator it = itg->second->mmapResource.find(strNewResourceName);		// Try to find the named resource in the group
		if (itg->second->mmapResource.end() != it)
		{
			it->second->uiReferenceCount++;
			return it->second->pResource;
		}

		// If we get here, we have got to create, then add the resource to the existing named group
		ResourceSample* pNewRes = new ResourceSample;
		pNewRes->pResource = new CAudioSample();
		pNewRes->pResource->_mstrAudioFilename = strNewResourceName;
		pNewRes->uiReferenceCount = 1;
		pNewRes->bLoaded = false;
		itg->second->mmapResource[strNewResourceName] = pNewRes;

		// Load in the wave format structure, not the whole sample data.
		// We do this so we can add emitters without having to load the entire sample data in first.
		pNewRes->pResource->loadFormat();

		return pNewRes->pResource;
	}

	CAudioSample* SCAudioManager::getSample(const String& strResourceName, const String& strGroupName)
	{
		// Group doesn't exist?
		if (!sampleGroupExists(strGroupName))
		{
			String err(L"AudioManager::getSample(\"");
			err.append(strResourceName);
			err += L"\", \"";
			err.append(strGroupName);
			err += L"\") failed. As the given named group of \"";
			err.append(strGroupName);
			err += L"\" doesn't exist.";
			ErrorIfTrue(1, err);
		}

		// Resource doesn't exist in the group?
		std::map<String, Group*>::iterator itg = _mmapGroup.find(strGroupName);							// Get iterator to the group (we know it exists)
		std::map<String, ResourceSample*>::iterator it = itg->second->mmapResource.find(strResourceName);	// Try to find the named resource in the group
		if (itg->second->mmapResource.end() == it)
		{
			String err(L"AudioManager::getSample(\"");
			err.append(strResourceName);
			err.appendWChar(L"\", \"");
			err.append(strGroupName);
			err.appendWChar(L"\") failed. Although the given named group of \"");
			err.append(strGroupName);
			err.appendWChar(L"\" exists, the named resource couldn't be found.");
			ErrorIfTrue(1, err);
		}

		// Is the resource in an unloaded state?
		if (!it->second->bLoaded)
		{
			// Load it
			it->second->pResource->load();
			it->second->bLoaded = true;
		}
		// Return the resource pointer...
		return it->second->pResource;
	}

	bool SCAudioManager::getExistsSample(const String& strResourceName, const String& strGroupName)
	{
		std::map<String, Group*>::iterator itg = _mmapGroup.find(strGroupName);
		if (itg == _mmapGroup.end())
			return false;
		std::map<String, ResourceSample*>::iterator it = itg->second->mmapResource.find(strResourceName);
		if (it == itg->second->mmapResource.end())
			return false;
		return true;
	}

	bool SCAudioManager::getSampleLoaded(const String& strResourceName, const String& strGroupName)
	{
		std::map<String, Group*>::iterator itg = _mmapGroup.find(strGroupName);
		if (itg == _mmapGroup.end())
			return false;
		std::map<String, ResourceSample*>::iterator it = itg->second->mmapResource.find(strResourceName);
		if (it == itg->second->mmapResource.end())
			return false;
		return it->second->bLoaded;
	}

	void SCAudioManager::removeSample(const String& strResourceName, const String& strGroupName)
	{
		// Group doesn't exist?
		if (!sampleGroupExists(strGroupName))
		{
			String err(L"AudioManager::removeSample(\"");
			err.append(strResourceName);
			err.appendWChar(L"\", \"");
			err.append(strGroupName);
			err.appendWChar(L"\") failed. As the given named group of \"");
			err.append(strGroupName);
			err.appendWChar(L"\" which the resource is supposed to be in, doesn't exist.");
			ErrorIfTrue(1, err);
		}

		// Resource doesn't exist in the group?
		std::map<String, Group*>::iterator itg = _mmapGroup.find(strGroupName);							// Get iterator to the group (we know it exists)
		std::map<String, ResourceSample*>::iterator it = itg->second->mmapResource.find(strResourceName);	// Try to find the named resource in the group
		if (itg->second->mmapResource.end() == it)
		{
			String err(L"AudioManager::removeSample(\"");
			err.append(strResourceName);
			err.appendWChar(L"\", \"");
			err.append(strGroupName);
			err.appendWChar(L"\") failed. Although the given named group of \"");
			err.append(strGroupName);
			err.appendWChar(L"\" which the resource is supposed to be in, exists, the named resource couldn't be found.");
			ErrorIfTrue(1, err);
		}

		// If we get here, we've found the resource in the group
		// Decrement it's reference count 
		it->second->uiReferenceCount--;
		// If the reference count is now at zero
		if (it->second->uiReferenceCount <= 0)
		{
			// If it's in a loaded state, unload it
			if (it->second->bLoaded)
			{
				it->second->pResource->unload();
				it->second->bLoaded = false;
			}

			// Destroy the resource
			delete it->second->pResource;
			delete it->second;
			itg->second->mmapResource.erase(it);
		}
	}

	unsigned int SCAudioManager::getMemoryUsage(void)
	{
		XAUDIO2_PERFORMANCE_DATA pPerformanceData;// = 0;
		_mpXAudio2->GetPerformanceData(&pPerformanceData);
		return pPerformanceData.MemoryUsageInBytes;
	}

	CAudioEmitter* SCAudioManager::addEmitter(const String& strEmitterName, const String& strSampleName, unsigned int iMaxSimultaneousInstances, const String& strSampleGroupname)
	{
		String strErr(L"AudioManager::addEmitter(");
		strErr += strEmitterName + L", " + strSampleName + L", ";
		strErr.appendUnsignedInt(iMaxSimultaneousInstances);
		strErr += L", " + strSampleGroupname + L") ";

		// Make sure the named group exists
		std::map<String, Group*>::iterator itg = _mmapGroup.find(strSampleGroupname);
		if (itg == _mmapGroup.end())
		{
			strErr += L"failed as the named group doesn't exist.";
			ErrorIfTrue(1, strErr);
		}
		// Make sure the named sample exists
		std::map<String, ResourceSample*>::iterator it = itg->second->mmapResource.find(strSampleName);
		if (it == itg->second->mmapResource.end())
		{
			strErr += L"failed as although the named group exists, the named sample doesn't.";
			ErrorIfTrue(1, strErr);
		}
		// If we get here, the sample was found in the named group.

		auto itEmitter = _mmapResourceEmitters.find(strEmitterName);
		CAudioEmitter* pEmitter = 0;
		if (itEmitter != _mmapResourceEmitters.end())
		{
			itEmitter->second->uiReferenceCount++;
			pEmitter = itEmitter->second->pResource;
		}
		else
		{
			// Create new resource
			ResourceEmitter* pNewResEmitter = new ResourceEmitter;
			ErrorIfMemoryNotAllocated(pNewResEmitter);

			// Create the new emitter object
			pNewResEmitter->uiReferenceCount = 1;
			pNewResEmitter->pResource = new CAudioEmitter(it->second->pResource->wfx, iMaxSimultaneousInstances);
			ErrorIfMemoryNotAllocated(pNewResEmitter->pResource);
			pEmitter = pNewResEmitter->pResource;	// Store pointer so we can return it below.

			// Store the sample's name and group within the emitter as emitters only work with their set sample data
			pNewResEmitter->pResource->_mstrSampleName = strSampleName;
			pNewResEmitter->pResource->_mstrSampleGroupname = strSampleGroupname;

			// Store it in the hash map
			_mmapResourceEmitters[strEmitterName] = pNewResEmitter;
		}
		return pEmitter;
	}

	void SCAudioManager::removeEmitter(const String& strEmitterName)
	{
		auto it = _mmapResourceEmitters.find(strEmitterName);
		if (it == _mmapResourceEmitters.end())
		{
			String strErr(L"AudioManager::removeEmitter(");
			strErr += strEmitterName;
			strErr += L") failed as the named emitter doesn't exist.";
			ErrorIfTrue(1, strErr);
		}
		it->second->uiReferenceCount--;
		if (it->second->uiReferenceCount == 0)
		{
			delete it->second->pResource;
			delete it->second;
			_mmapResourceEmitters.erase(it);
		}
	}

	bool SCAudioManager::getExistsEmitter(const String& strEmitterName)
	{
		auto it = _mmapResourceEmitters.find(strEmitterName);
		if (it == _mmapResourceEmitters.end())
			return false;
		return true;
	}

	CAudioEmitter* SCAudioManager::getEmitter(const String& strEmitterName)
	{
		auto it = _mmapResourceEmitters.find(strEmitterName);
		if (it == _mmapResourceEmitters.end())
		{
			String strErr(L"AudioManager::getEmitter(");
			strErr += strEmitterName;
			strErr += L") failed as the named emitter doesn't exist.";
			ErrorIfTrue(1, strErr);
		}
		return it->second->pResource;
	}
}