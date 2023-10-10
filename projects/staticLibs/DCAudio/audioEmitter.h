#pragma once
#include <vector>

// XAudio2
#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif
#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif
//#include <x3daudio.h>
//#include <xapo.h>
//#include <xapobase.h>
//#include <xapofx.h>
#include <xaudio2.h>
//#include <xaudio2fx.h>
//#include <hrtfapoapi.h>
//#include <mmdeviceapi.h>

#include "../DCCommon/string.h"

namespace DC
{
	// An audio emitter which emits sounds from loaded audio samples
	class AudioEmitter
	{
		friend class SCAudioManager;
	public:
		AudioEmitter(WAVEFORMATEXTENSIBLE& wfx, unsigned int maxSimultaneousInstances = 8);
		~AudioEmitter();

		// Plays an instance for this emitter using given values
		// If the sample isn't currently loaded, this silently fails
		void play(float volume = 1.0f, float playbackSpeed = 1.0f, bool loop = false);

		// Stops playback of all voices for this emitter
		void stopAll(void);

		// Stops playback of a voice at the specified index.
		// The index can be in the range of 0 to maximum number of simultaneous instances, minus one.
		// If an invalid index is given, an exception occurs.
		void stop(unsigned int index);

		// Returns the number of voices which are currently playing for this emitter
		unsigned int getNumVoicesPlaying(void) const;

		// Sets a voice's volume whilst it's playing
		// If an invalid index is given, an exception occurs
		void setVolume(unsigned int index, float volume);

		// Retrieves a voice's currently set volume
		// If an invalid index is given, an exception occurs
		float getVolume(unsigned int index) const;

		// Sets a voice's frequency whilst it's playing
		// If an invalid index is given, an exception occurs
		void setFrequency(unsigned int index, float volume);

		// Retrieves a voice's currently set frequency
		// If an invalid index is given, an exception occurs
		float getFrequency(unsigned int index) const;

	private:
		unsigned int maxSimultaneousInstances;		// Maximum number of sounds which can be played back simultaneously
		std::vector<IXAudio2SourceVoice*> voices;	// Multiple instances playback
		unsigned int voicesIndex;					// Which voice index to play next
		String sampleName;							// The named sample which this emitter uses for playback
		String sampleGroupname;						// The named sample's groupname which it is located within.
	};


}