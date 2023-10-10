#pragma once
#include "audioSample.h"
#include "audioEmitter.h"
#include <map>
#include "../DCCommon/singleton.h"

namespace DC
{
	// Use this to manage all audio samples and their playback via audio emitters
	// Note: Only supports playback of samples in the ".wav" format from disk.
	// To be honest, I doubt I'll add playback of .ogg or .mp3 as .wav is uncompressed with better audio quality, takes less time to decompress (as there isn't any) and 
	// storage space isn't too much of an issue, say for example an hour's worth of music only takes up around 600MB anyway.
	// I also found when playing around with other libraries such as FMOD or BASS that the loop points with compressed file formats were never precise enough, causing an
	// ever so slightly audible click or weird artifact with looping samples. .wav doesn't have this issue!
	// Who knows though, never say never!
	// Usage:
	// Audio sample data is stored inside groups
	// By default, a group named "default" is created for storage of audio samples upon construction.
	// Add a new sample to the "default" group
	// pAudioManager->addSample("audio/fart.wav", "default");
	// // Add more samples if needed and then call..
	// pAudioManager->loadSampleGroup("default");	// Load all audio data, so it's ready for use
	// Create an emitter which will be used to playback a sample.
	// Emitters are "linked" to a single sample, they can only play that one sample
	// AudioEmitter *pAudioEmitter = pAudioManager->addEmitter("MyBottomEmitter", "audio/fart.wav", 8, "default");
	// Now we can playback the sample with...
	// pAudioEmitter->play(1.0f, 1.0f, false);	// Where parameters are volume, playback speed and whether to loop the sample or not
	class SCAudioManager : public Singleton<SCAudioManager>
	{
	public:
		friend class AudioEmitter;
		SCAudioManager();

		// Return the number of sample resource groups which currently exist in the manager
		unsigned int getNumSampleGroups(void);

		// Returns the number of samples which currently exist in the named group
		// If the named group doesn't exist, an exception occurs
		unsigned int getNumSamplesInGroup(const String& strGroupName);

		// Returns the number of samples which are currently loaded in the named group
		// If the named group doesn't exist, an exception occurs
		unsigned int getNumSamplesInGroupLoaded(const String& strGroupName);

		// Returns the sample group name, at specified index
		// If invalid index given, an exception occurs
		const String& getSampleGroupName(unsigned int iGroupIndex);

		// Adds a new sample resource group with the given name which resources can be put into
		// If the named group already exists, this simply returns
		void addNewSampleGroup(const String& strNewGroupName);

		// Returns true if the named sample group exists, else false
		bool sampleGroupExists(const String& strGroupName);

		// Will load all sample resources in the named group so that they're ready for use.
		// If the named group doesn't exist, an exception occurs
		// If the group was previously loaded, then some more resources were added, only the newly added resources will be loaded
		// as the previous resources will have already been loaded.
		void loadSampleGroup(const String& strGroupName);

		// Will load a single unloaded sample resource in the named group so that it's ready for use.
		// This will load only a single unloaded resource within the named group, then return.
		// This method is usefull for creating loading screens, where you call this, then call getNumResInGroup and getNumResInGroupLoaded, when both return values from
		// these two methods are the same, all resources are loaded.
		// If the named group doesn't exist, an exception occurs
		// If the group was previously loaded, then some more resources were added, only the newly added resources will be loaded
		// as the previous resources will have already been loaded.
		void loadSampleGroupSingle(const String& strGroupName);

		// Will unload all sample resources in the named group to free up memory.
		// If the named group doesn't exist, an exception occurs
		void unloadSampleGroup(const String& strGroupName);

		// Adds a new sample resource to the named group
		// If the group name doesn't exist, an exception occurs.
		// If the resource name already exists, the resource's reference count is increased
		// If the resource doesn't previously exist and it's newly created, it'll be in it's unloaded state
		// strNewResourceName is used to refer to the sample data by name and is also the filename holding the sample's sample data
		AudioSample* addSample(const String& strNewResourceName, const String& strGroupName = L"default");

		// Returns a pointer to the named sample resource in it's named group
		// If either the group given doesn't exist, or the named resource doesn't exist, an exception occurs
		// Also, if the resource is in the unloaded state, it is loaded here
		AudioSample* getSample(const String& strResourceName, const String& strGroupName = L"default");

		// Returns true if the named sample resource exists in the named group, else false
		bool getExistsSample(const String& strResourceName, const String& strGroupName = L"default");

		// Returns true if the named sample is currently loaded
		// Returns false if the sample couldn't be found
		bool getSampleLoaded(const String& strResourceName, const String& strGroupName = L"default");

		// Removes the named sample resource from the named group
		// If either the resource or the group that it's in doesn't exist, an exception occurs
		// Remove all emitters that are playing this sample before calling this.
		void removeSample(const String& strResourceName, const String& strGroupName);

		// Returns memory usage in bytes
		unsigned int getMemoryUsage(void);

		// Adds an emitter which we use to playback the named sample.
		// If the named sample doesn't exist, an exception occurs
		// If the resource name already exists, the resource's reference count is increased
		AudioEmitter* addEmitter(const String& strEmitterName, const String& strSampleName, unsigned int iMaxSimultaneousInstances = 8, const String& strSampleGroupname = L"default");

		// Removes the named emitter and stops playback
		// If either the resource or the group that it's in doesn't exist, an exception occurs
		// Will only remove the emitter if it's reference count is zero
		void removeEmitter(const String& strEmitterName);

		// Returns whether the named emitter already exists
		bool getExistsEmitter(const String& strEmitterName);

		// Returns a pointer to the named emitter
		// If the named emitter doesn't exist, an exception occurs
		AudioEmitter* getEmitter(const String& strEmitterName);
	private:

		// A resource and various variables needed by the manager for each resource
		struct ResourceSample
		{
			AudioSample* resource;			// Pointer to the resource
			unsigned int referenceCount;	// How many times the resource has been added/removed
			bool loaded;					// Whether the resource is loaded or not
		};
		// An audio sample resource group holding each resource
		struct Group
		{
			std::map<String, ResourceSample*> resources;	// Hash map holding named resource
		};
		std::map<String, Group*> groups;					// Hash map holding named resource groups

		IXAudio2* XAudio2;							// Main XAudio2 interface
		IXAudio2MasteringVoice* masteringVoice;

		struct ResourceEmitter
		{
			AudioEmitter* resource;			// Pointer to the resource
			unsigned int referenceCount;	// How many times the resource has been added/removed
		};
		std::map<String, ResourceEmitter*> resourceEmitters;		// Holds each named emitter
	};

}