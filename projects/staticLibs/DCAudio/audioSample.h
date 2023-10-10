#pragma once
#include "../DCCommon/string.h"
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

namespace DC
{
	// An audio sample containing sample data used for playback via audio emitters
	class AudioSample
	{
		friend class SCAudioManager;
		friend class AudioEmitter;
	public:
		AudioSample();
		~AudioSample();

		// Loads the resource so it's ready for use.
		// Used by the manager
		void load(void);

		// Loads the sample's wave format into wfx
		// Used by the manager.
		// This doesn't load in all the sample data, just the sample's format.
		// This is so the sample doesn't have to be loaded fully before setting up emitters
		void loadFormat(void);

		// Unloads the resource, freeing memory.
		// Used by the manager 
		void unload(void);

	private:
		String audioFilename;		// Holds the name of the file which holds the sample data

		HRESULT findChunk(HANDLE file, DWORD fourcc, DWORD& chunkSize, DWORD& chunkDataPosition);
		HRESULT readChunkData(HANDLE file, void* buffer, DWORD buffersize, DWORD bufferoffset);
		XAUDIO2_BUFFER buffer = { 0 };		// Holds the sample data once the object is loaded
		WAVEFORMATEXTENSIBLE wfx = { 0 };	// Holds the information about the sample data once loadFormat() has been called
	};
}