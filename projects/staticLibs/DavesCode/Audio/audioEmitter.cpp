#include "audioEmitter.h"
#include "audioManager.h"
#include "../Common/error.h"

namespace DC
{
	AudioEmitter::AudioEmitter(WAVEFORMATEXTENSIBLE& wfx, unsigned int maxSimultaneousInstances)
	{
		ErrorIfTrue(0 == maxSimultaneousInstances, L"AudioEmitter::AudioEmitter() given 0 iMaxSimultaneousInstances,");
		maxSimultaneousInstances = maxSimultaneousInstances;
		voicesIndex = 0;
		// Create voices for the audio emitter
		HRESULT hr;
		for (int iVoice = 0; iVoice < (int)maxSimultaneousInstances; iVoice++)
		{
			IXAudio2SourceVoice* pVoice;
			// Create a source voice by calling the IXAudio2::CreateSourceVoice method on an instance of the XAudio2 engine.
			// The format of the voice is specified by the values set in a WAVEFORMATEX structure.
			ErrorIfTrue(FAILED(hr = SCAudioManager::getPointer()->XAudio2->CreateSourceVoice(&pVoice, (WAVEFORMATEX*)&wfx)), L"AudioEmitter::AudioEmitter() failed to create source voice.");
			voices.push_back(pVoice);
		}
	}

	AudioEmitter::~AudioEmitter()
	{
		stopAll();	// Stop playback of all voices for this emitter
		for (int iVoice = 0; iVoice < voices.size(); iVoice++)
		{
			voices[iVoice]->DestroyVoice();
		}
	}

	void AudioEmitter::play(float volume, float playbackSpeed, bool loop)
	{
		// Find the sample which this emitter uses
		if (!SCAudioManager::getPointer()->getExistsSample(sampleName, sampleGroupname))
		{
			String strErr(L"AudioEmitter::play() failed as it's set sample of ");
			strErr += sampleName + L" in group " + sampleGroupname + L" doesn't exist in the audio manager.";
			ErrorIfTrue(1, strErr);
		}
		// Make sure the audio sample is loaded
		if (false == SCAudioManager::getPointer()->getSampleLoaded(sampleName, sampleGroupname))
			return;

		// Before playing from the current voice index, stop it from playing incase it is so
		IXAudio2SourceVoice* pSourceVoice = voices[voicesIndex];
		pSourceVoice->Stop();
		pSourceVoice->FlushSourceBuffers();

		// Get pointer to the audio sample
		AudioSample* pAudioSample = SCAudioManager::getPointer()->getSample(sampleName, sampleGroupname);

		// Set the sample's loop variables
		if (loop)	// Loop the thing?
		{
			pAudioSample->buffer.LoopBegin = 0;
			pAudioSample->buffer.LoopLength = 0;
			pAudioSample->buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
		}
		else
		{
			pAudioSample->buffer.LoopBegin = 0;
			pAudioSample->buffer.LoopLength = 0;
			pAudioSample->buffer.LoopCount = 0;
		}
		HRESULT hr;
		hr = pSourceVoice->SubmitSourceBuffer(&pAudioSample->buffer);
		ErrorIfTrue(FAILED(hr), L"AudioEmitter::play() failed. Error submitting source buffer.");

		// Set voice volume, pan and frequency
		pSourceVoice->SetVolume(volume);
		pSourceVoice->SetFrequencyRatio(playbackSpeed);

		hr = pSourceVoice->Start(0);
		ErrorIfTrue(FAILED(hr), L"AudioEmitter::play() failed during call to pSourceVoice->Start()");

		// Deal with voice index
		voicesIndex++;
		if (voicesIndex >= voices.size())
			voicesIndex = 0;
	}

	void AudioEmitter::stopAll(void)
	{
		HRESULT hr;
		for (int iVoice = 0; iVoice < voices.size(); iVoice++)
		{
			hr = voices[iVoice]->Stop();	// Stop playback of this voice
			ErrorIfTrue(FAILED(hr), L"AudioEmitter::stopAll() failed. Error calling IXAudio2SourceVoice->Stop().");
			hr = voices[iVoice]->FlushSourceBuffers();
			ErrorIfTrue(FAILED(hr), L"AudioEmitter::stopAll() failed. Error calling IXAudio2SourceVoice->FlushSourceBuffers().");
		}
	}

	void AudioEmitter::stop(unsigned int index)
	{
		// Make sure valid index is given
		if (index >= maxSimultaneousInstances)
		{
			String strErr(L"AudioEmitter::stop(");
			strErr.appendUnsignedInt(index);
			strErr += L") failed as given index has to be less than ";
			strErr.appendUnsignedInt(maxSimultaneousInstances);
			ErrorIfTrue(1, strErr);
		}

		HRESULT hr;
		hr = voices[index]->Stop();	// Stop playback of this voice
		ErrorIfTrue(FAILED(hr), L"AudioEmitter::stop() failed. Error calling IXAudio2SourceVoice->Stop().");
		hr = voices[index]->FlushSourceBuffers();
		ErrorIfTrue(FAILED(hr), L"AudioEmitter::stop() failed. Error calling IXAudio2SourceVoice->FlushSourceBuffers().");
	}

	unsigned int AudioEmitter::getNumVoicesPlaying(void) const
	{
		unsigned int iNumberVoicesPlaying = 0;
		XAUDIO2_VOICE_STATE state;
		for (int i = 0; i < voices.size(); i++)
		{
			voices[i]->GetState(&state);
			if (state.BuffersQueued > 0)
				iNumberVoicesPlaying++;
		}
		return iNumberVoicesPlaying;
	}

	void AudioEmitter::setVolume(unsigned int index, float volume)
	{
		// Make sure valid index is given
		if (index >= maxSimultaneousInstances)
		{
			String strErr(L"AudioEmitter::setVolume(");
			strErr.appendUnsignedInt(index);
			strErr += L") failed as given index has to be less than ";
			strErr.appendUnsignedInt(maxSimultaneousInstances);
			ErrorIfTrue(1, strErr);
		}
		voices[index]->SetVolume(volume);
	}

	float AudioEmitter::getVolume(unsigned int index) const
	{
		// Make sure valid index is given
		if (index >= maxSimultaneousInstances)
		{
			String strErr(L"AudioEmitter::getVolume(");
			strErr.appendUnsignedInt(index);
			strErr += L") failed as given index has to be less than ";
			strErr.appendUnsignedInt(maxSimultaneousInstances);
			ErrorIfTrue(1, strErr);
		}
		float fVolume = 0.0f;
		voices[index]->GetVolume(&fVolume);
		return fVolume;
	}

	void AudioEmitter::setFrequency(unsigned int index, float frequency)
	{
		// Make sure valid index is given
		if (index >= maxSimultaneousInstances)
		{
			String strErr(L"AudioEmitter::setFrequency(");
			strErr.appendUnsignedInt(index);
			strErr += L") failed as given index has to be less than ";
			strErr.appendUnsignedInt(maxSimultaneousInstances);
			ErrorIfTrue(1, strErr);
		}
		voices[index]->SetFrequencyRatio(frequency);
	}

	float AudioEmitter::getFrequency(unsigned int index) const
	{
		// Make sure valid index is given
		if (index >= maxSimultaneousInstances)
		{
			String strErr(L"AudioEmitter::getFrequency(");
			strErr.appendUnsignedInt(index);
			strErr += L") failed as given index has to be less than ";
			strErr.appendUnsignedInt(maxSimultaneousInstances);
			ErrorIfTrue(1, strErr);
		}
		float fFrequency = 0.0f;
		voices[index]->GetFrequencyRatio(&fFrequency);
		return fFrequency;
	}
}