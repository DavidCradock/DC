#include "audioManager.h"
#include "../Common/error.h"

namespace DC
{
	SCAudioManager::SCAudioManager()
	{
		HRESULT hr;
		hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		ErrorIfTrue(FAILED(hr), L"AudioManager() failed to initialise COM.");

		XAudio2 = nullptr;
		ErrorIfTrue(FAILED(hr = XAudio2Create(&XAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)), L"AudioManager() failed to create instance of the XAudio2 engine.");

		masteringVoice = nullptr;
		ErrorIfTrue(FAILED(hr = XAudio2->CreateMasteringVoice(&masteringVoice)), L"AudioManager() failed to create mastering voice.");

		addNewSampleGroup(L"default");
	}

	unsigned int SCAudioManager::getNumSampleGroups(void)
	{
		return (unsigned int)groups.size();
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
		std::map<String, Group*>::iterator itg = groups.find(strGroupName);
		return (unsigned int)itg->second->resources.size();;
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
		std::map<String, Group*>::iterator itg = groups.find(strGroupName);

		unsigned int iResLoadedTotal = 0;
		std::map<String, ResourceSample*>::iterator it = itg->second->resources.begin();
		while (it != itg->second->resources.end())
		{
			if (it->second->loaded)
				++iResLoadedTotal;
			it++;
		}
		return iResLoadedTotal;
	}

	const String& SCAudioManager::getSampleGroupName(unsigned int iGroupIndex)
	{
		if (iGroupIndex >= groups.size())
		{
			String err(L"AudioManager::getSampleGroupName(");
			
			err.appendUnsignedInt(iGroupIndex);
			err.append(L") failed. Invalid index given. Number of groups is ");
			err.appendUnsignedInt(getNumSampleGroups());
			ErrorIfTrue(1, err);
		}
		std::map<String, Group*>::iterator itg = groups.begin();
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
		groups[strNewGroupName] = pNewGroup;
	}

	bool SCAudioManager::sampleGroupExists(const String& strGroupName)
	{
		std::map<String, Group*>::iterator it = groups.find(strGroupName);
		if (it == groups.end())
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
		std::map<String, Group*>::iterator itg = groups.find(strGroupName);

		// For each resource in this group...
		std::map<String, ResourceSample*>::iterator it = itg->second->resources.begin();
		while (it != itg->second->resources.end())
		{
			if (!it->second->loaded)
			{
				it->second->resource->load();
				it->second->loaded = true;
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
		std::map<String, Group*>::iterator itg = groups.find(strGroupName);

		// For each resource in this group...
		std::map<String, ResourceSample*>::iterator it = itg->second->resources.begin();
		while (it != itg->second->resources.end())
		{
			if (!it->second->loaded)
			{
				it->second->resource->load();
				it->second->loaded = true;
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
		std::map<String, Group*>::iterator itg = groups.find(strGroupName);

		// For each resource in this group...
		std::map<String, ResourceSample*>::iterator it = itg->second->resources.begin();
		while (it != itg->second->resources.end())
		{
			if (it->second->loaded)
			{
				it->second->resource->unload();
				it->second->loaded = false;
			}
			it++;
		}
	}

	AudioSample* SCAudioManager::addSample(const String& strNewResourceName, const String& strGroupName)
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
		std::map<String, Group*>::iterator itg = groups.find(strGroupName);									// Get iterator to the group (we know it exists)
		std::map<String, ResourceSample*>::iterator it = itg->second->resources.find(strNewResourceName);		// Try to find the named resource in the group
		if (itg->second->resources.end() != it)
		{
			it->second->referenceCount++;
			return it->second->resource;
		}

		// If we get here, we have got to create, then add the resource to the existing named group
		ResourceSample* pNewRes = new ResourceSample;
		pNewRes->resource = new AudioSample();
		pNewRes->resource->audioFilename = strNewResourceName;
		pNewRes->referenceCount = 1;
		pNewRes->loaded = false;
		itg->second->resources[strNewResourceName] = pNewRes;

		// Load in the wave format structure, not the whole sample data.
		// We do this so we can add emitters without having to load the entire sample data in first.
		pNewRes->resource->loadFormat();

		return pNewRes->resource;
	}

	AudioSample* SCAudioManager::getSample(const String& strResourceName, const String& strGroupName)
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
		std::map<String, Group*>::iterator itg = groups.find(strGroupName);								// Get iterator to the group (we know it exists)
		std::map<String, ResourceSample*>::iterator it = itg->second->resources.find(strResourceName);	// Try to find the named resource in the group
		if (itg->second->resources.end() == it)
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
		if (!it->second->loaded)
		{
			// Load it
			it->second->resource->load();
			it->second->loaded = true;
		}
		// Return the resource pointer...
		return it->second->resource;
	}

	bool SCAudioManager::getExistsSample(const String& strResourceName, const String& strGroupName)
	{
		std::map<String, Group*>::iterator itg = groups.find(strGroupName);
		if (itg == groups.end())
			return false;
		std::map<String, ResourceSample*>::iterator it = itg->second->resources.find(strResourceName);
		if (it == itg->second->resources.end())
			return false;
		return true;
	}

	bool SCAudioManager::getSampleLoaded(const String& strResourceName, const String& strGroupName)
	{
		std::map<String, Group*>::iterator itg = groups.find(strGroupName);
		if (itg == groups.end())
			return false;
		std::map<String, ResourceSample*>::iterator it = itg->second->resources.find(strResourceName);
		if (it == itg->second->resources.end())
			return false;
		return it->second->loaded;
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
		std::map<String, Group*>::iterator itg = groups.find(strGroupName);								// Get iterator to the group (we know it exists)
		std::map<String, ResourceSample*>::iterator it = itg->second->resources.find(strResourceName);	// Try to find the named resource in the group
		if (itg->second->resources.end() == it)
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
		it->second->referenceCount--;
		// If the reference count is now at zero
		if (it->second->referenceCount <= 0)
		{
			// If it's in a loaded state, unload it
			if (it->second->loaded)
			{
				it->second->resource->unload();
				it->second->loaded = false;
			}

			// Destroy the resource
			delete it->second->resource;
			delete it->second;
			itg->second->resources.erase(it);
		}
	}

	unsigned int SCAudioManager::getMemoryUsage(void)
	{
		XAUDIO2_PERFORMANCE_DATA pPerformanceData;// = 0;
		XAudio2->GetPerformanceData(&pPerformanceData);
		return pPerformanceData.MemoryUsageInBytes;
	}

	AudioEmitter* SCAudioManager::addEmitter(const String& strEmitterName, const String& strSampleName, unsigned int iMaxSimultaneousInstances, const String& strSampleGroupname)
	{
		String strErr(L"AudioManager::addEmitter(");
		strErr += strEmitterName + L", " + strSampleName + L", ";
		strErr.appendUnsignedInt(iMaxSimultaneousInstances);
		strErr += L", " + strSampleGroupname + L") ";

		// Make sure the named group exists
		std::map<String, Group*>::iterator itg = groups.find(strSampleGroupname);
		if (itg == groups.end())
		{
			strErr += L"failed as the named group doesn't exist.";
			ErrorIfTrue(1, strErr);
		}
		// Make sure the named sample exists
		std::map<String, ResourceSample*>::iterator it = itg->second->resources.find(strSampleName);
		if (it == itg->second->resources.end())
		{
			strErr += L"failed as although the named group exists, the named sample doesn't.";
			ErrorIfTrue(1, strErr);
		}
		// If we get here, the sample was found in the named group.

		auto itEmitter = resourceEmitters.find(strEmitterName);
		AudioEmitter* pEmitter = 0;
		if (itEmitter != resourceEmitters.end())
		{
			itEmitter->second->referenceCount++;
			pEmitter = itEmitter->second->resource;
		}
		else
		{
			// Create new resource
			ResourceEmitter* pNewResEmitter = new ResourceEmitter;
			ErrorIfMemoryNotAllocated(pNewResEmitter);

			// Create the new emitter object
			pNewResEmitter->referenceCount = 1;
			pNewResEmitter->resource = new AudioEmitter(it->second->resource->wfx, iMaxSimultaneousInstances);
			ErrorIfMemoryNotAllocated(pNewResEmitter->resource);
			pEmitter = pNewResEmitter->resource;	// Store pointer so we can return it below.

			// Store the sample's name and group within the emitter as emitters only work with their set sample data
			pNewResEmitter->resource->sampleName = strSampleName;
			pNewResEmitter->resource->sampleGroupname = strSampleGroupname;

			// Store it in the hash map
			resourceEmitters[strEmitterName] = pNewResEmitter;
		}
		return pEmitter;
	}

	void SCAudioManager::removeEmitter(const String& strEmitterName)
	{
		auto it = resourceEmitters.find(strEmitterName);
		if (it == resourceEmitters.end())
		{
			String strErr(L"AudioManager::removeEmitter(");
			strErr += strEmitterName;
			strErr += L") failed as the named emitter doesn't exist.";
			ErrorIfTrue(1, strErr);
		}
		it->second->referenceCount--;
		if (it->second->referenceCount == 0)
		{
			delete it->second->resource;
			delete it->second;
			resourceEmitters.erase(it);
		}
	}

	bool SCAudioManager::getExistsEmitter(const String& strEmitterName)
	{
		auto it = resourceEmitters.find(strEmitterName);
		if (it == resourceEmitters.end())
			return false;
		return true;
	}

	AudioEmitter* SCAudioManager::getEmitter(const String& strEmitterName)
	{
		auto it = resourceEmitters.find(strEmitterName);
		if (it == resourceEmitters.end())
		{
			String strErr(L"AudioManager::getEmitter(");
			strErr += strEmitterName;
			strErr += L") failed as the named emitter doesn't exist.";
			ErrorIfTrue(1, strErr);
		}
		return it->second->resource;
	}
}