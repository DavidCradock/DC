#include "audioEmitter.h"
#include "audioManager.h"
#include "../DCCommon/error.h"

namespace DC
{
	CAudioEmitter::CAudioEmitter(WAVEFORMATEXTENSIBLE& wfx, unsigned int iMaxSimultaneousInstances)
	{
		ErrorIfTrue(0 == iMaxSimultaneousInstances, L"AudioEmitter::CAudioEmitter() given 0 iMaxSimultaneousInstances,");
		_muiMaxSimultaneousInstances = iMaxSimultaneousInstances;
		_muiVecVoicesIndex = 0;
		// Create voices for the audio emitter
		HRESULT hr;
		for (int iVoice = 0; iVoice < (int)iMaxSimultaneousInstances; iVoice++)
		{
			IXAudio2SourceVoice* pVoice;
			// Create a source voice by calling the IXAudio2::CreateSourceVoice method on an instance of the XAudio2 engine.
			// The format of the voice is specified by the values set in a WAVEFORMATEX structure.
			ErrorIfTrue(FAILED(hr = SCAudioManager::getPointer()->_mpXAudio2->CreateSourceVoice(&pVoice, (WAVEFORMATEX*)&wfx)), L"AudioEmitter::CAudioEmitter() failed to create source voice.");
			_mvecVoices.push_back(pVoice);
		}
	}

	CAudioEmitter::~CAudioEmitter()
	{
		stopAll();	// Stop playback of all voices for this emitter
		for (int iVoice = 0; iVoice < _mvecVoices.size(); iVoice++)
		{
			_mvecVoices[iVoice]->DestroyVoice();
		}
	}

	void CAudioEmitter::play(float fVolume, float fPlaybackSpeed, bool bLoop)
	{
		// Find the sample which this emitter uses
		if (!SCAudioManager::getPointer()->getExistsSample(_mstrSampleName, _mstrSampleGroupname))
		{
			String strErr(L"AudioEmitter::play() failed as it's set sample of ");
			strErr += _mstrSampleName + L" in group " + _mstrSampleGroupname + L" doesn't exist in the audio manager.";
			ErrorIfTrue(1, strErr);
		}
		// Make sure the audio sample is loaded
		if (false == SCAudioManager::getPointer()->getSampleLoaded(_mstrSampleName, _mstrSampleGroupname))
			return;

		// Before playing from the current voice index, stop it from playing incase it is so
		IXAudio2SourceVoice* pSourceVoice = _mvecVoices[_muiVecVoicesIndex];
		pSourceVoice->Stop();
		pSourceVoice->FlushSourceBuffers();

		// Get pointer to the audio sample
		CAudioSample* pAudioSample = SCAudioManager::getPointer()->getSample(_mstrSampleName, _mstrSampleGroupname);

		// Set the sample's loop variables
		if (bLoop)	// Loop the thing?
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
		pSourceVoice->SetVolume(fVolume);
		pSourceVoice->SetFrequencyRatio(fPlaybackSpeed);

		hr = pSourceVoice->Start(0);
		ErrorIfTrue(FAILED(hr), L"AudioEmitter::play() failed during call to pSourceVoice->Start()");

		// Deal with voice index
		_muiVecVoicesIndex++;
		if (_muiVecVoicesIndex >= _mvecVoices.size())
			_muiVecVoicesIndex = 0;
	}

	void CAudioEmitter::stopAll(void)
	{
		HRESULT hr;
		for (int iVoice = 0; iVoice < _mvecVoices.size(); iVoice++)
		{
			hr = _mvecVoices[iVoice]->Stop();	// Stop playback of this voice
			ErrorIfTrue(FAILED(hr), L"AudioEmitter::stopAll() failed. Error calling IXAudio2SourceVoice->Stop().");
			hr = _mvecVoices[iVoice]->FlushSourceBuffers();
			ErrorIfTrue(FAILED(hr), L"AudioEmitter::stopAll() failed. Error calling IXAudio2SourceVoice->FlushSourceBuffers().");
		}
	}

	void CAudioEmitter::stop(unsigned int uiIndex)
	{
		// Make sure valid index is given
		if (uiIndex >= _muiMaxSimultaneousInstances)
		{
			String strErr(L"AudioEmitter::stop(");
			strErr.appendUnsignedInt(uiIndex);
			strErr += L") failed as given index has to be less than ";
			strErr.appendUnsignedInt(_muiMaxSimultaneousInstances);
			ErrorIfTrue(1, strErr);
		}

		HRESULT hr;
		hr = _mvecVoices[uiIndex]->Stop();	// Stop playback of this voice
		ErrorIfTrue(FAILED(hr), L"AudioEmitter::stop() failed. Error calling IXAudio2SourceVoice->Stop().");
		hr = _mvecVoices[uiIndex]->FlushSourceBuffers();
		ErrorIfTrue(FAILED(hr), L"AudioEmitter::stop() failed. Error calling IXAudio2SourceVoice->FlushSourceBuffers().");
	}

	unsigned int CAudioEmitter::getNumVoicesPlaying(void) const
	{
		unsigned int iNumberVoicesPlaying = 0;
		XAUDIO2_VOICE_STATE state;
		for (int i = 0; i < _mvecVoices.size(); i++)
		{
			_mvecVoices[i]->GetState(&state);
			if (state.BuffersQueued > 0)
				iNumberVoicesPlaying++;
		}
		return iNumberVoicesPlaying;
	}

	void CAudioEmitter::setVolume(unsigned int uiIndex, float fVolume)
	{
		// Make sure valid index is given
		if (uiIndex >= _muiMaxSimultaneousInstances)
		{
			String strErr(L"AudioEmitter::setVolume(");
			strErr.appendUnsignedInt(uiIndex);
			strErr += L") failed as given index has to be less than ";
			strErr.appendUnsignedInt(_muiMaxSimultaneousInstances);
			ErrorIfTrue(1, strErr);
		}
		_mvecVoices[uiIndex]->SetVolume(fVolume);
	}

	float CAudioEmitter::getVolume(unsigned int uiIndex) const
	{
		// Make sure valid index is given
		if (uiIndex >= _muiMaxSimultaneousInstances)
		{
			String strErr(L"AudioEmitter::getVolume(");
			strErr.appendUnsignedInt(uiIndex);
			strErr += L") failed as given index has to be less than ";
			strErr.appendUnsignedInt(_muiMaxSimultaneousInstances);
			ErrorIfTrue(1, strErr);
		}
		float fVolume = 0.0f;
		_mvecVoices[uiIndex]->GetVolume(&fVolume);
		return fVolume;
	}

	void CAudioEmitter::setFrequency(unsigned int uiIndex, float fFrequency)
	{
		// Make sure valid index is given
		if (uiIndex >= _muiMaxSimultaneousInstances)
		{
			String strErr(L"AudioEmitter::setFrequency(");
			strErr.appendUnsignedInt(uiIndex);
			strErr += L") failed as given index has to be less than ";
			strErr.appendUnsignedInt(_muiMaxSimultaneousInstances);
			ErrorIfTrue(1, strErr);
		}
		_mvecVoices[uiIndex]->SetFrequencyRatio(fFrequency);
	}

	float CAudioEmitter::getFrequency(unsigned int uiIndex) const
	{
		// Make sure valid index is given
		if (uiIndex >= _muiMaxSimultaneousInstances)
		{
			String strErr(L"AudioEmitter::getFrequency(");
			strErr.appendUnsignedInt(uiIndex);
			strErr += L") failed as given index has to be less than ";
			strErr.appendUnsignedInt(_muiMaxSimultaneousInstances);
			ErrorIfTrue(1, strErr);
		}
		float fFrequency = 0.0f;
		_mvecVoices[uiIndex]->GetFrequencyRatio(&fFrequency);
		return fFrequency;
	}
}