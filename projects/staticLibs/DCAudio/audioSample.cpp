#include "audioSample.h"
#include "../DCCommon/error.h"
#include <Windows.h>

namespace DC
{
	AudioSample::AudioSample(void)
	{
		buffer.pAudioData = nullptr;
	}

	AudioSample::~AudioSample(void)
	{
		unload();
	}

	void AudioSample::load(void)
	{
		// Make sure filename has non-zero size
		ErrorIfFalse(bool(audioFilename.size() > 0), L"AudioSample::load() failed as filename of the audio sample data has zero size.");
		// Open the file
		HANDLE hFile = CreateFile(audioFilename.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		ErrorIfTrue(INVALID_HANDLE_VALUE == hFile, L"AudioSample::load() failed. Invalid file handle.");

		ErrorIfTrue(INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN), L"AudioSample::load() failed. Invalid set file pointer.");

		// Locate the 'RIFF' chunk in the audio file, and check the file type.
		DWORD dwChunkSize;
		DWORD dwChunkPosition;
		//check the file type, should be fourccWAVE or 'XWMA'
		findChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
		DWORD filetype;
		readChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
		ErrorIfTrue(filetype != fourccWAVE, L"AudioSample::load() failed. File type is not wave.");

		// Locate the 'fmt ' chunk, and copy its contents into a WAVEFORMATEXTENSIBLE structure.
		findChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
		readChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

		// Locate the 'data' chunk, and read its contents into a buffer.
		//fill out the audio data buffer with the contents of the fourccDATA chunk
		findChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
		BYTE* pDataBuffer = new BYTE[dwChunkSize];
		readChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

		// Populate an XAUDIO2_BUFFER structure.
		buffer.AudioBytes = dwChunkSize;        // size of the audio buffer in bytes
		buffer.pAudioData = pDataBuffer;        // buffer containing audio data
		buffer.Flags = XAUDIO2_END_OF_STREAM;   // tell the source voice not to expect any data after this buffer
	}

	void AudioSample::loadFormat(void)
	{
		// Make sure filename has non-zero size
		ErrorIfFalse(bool(audioFilename.size() > 0), L"AudioSample::loadFormat() failed as filename of the audio sample data has zero size.");
		// Open the file
		HANDLE hFile = CreateFile(
			audioFilename.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		ErrorIfTrue(INVALID_HANDLE_VALUE == hFile, L"AudioSample::loadFormat() failed. Invalid file handle.");
		ErrorIfTrue(INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN), L"AudioSample::loadFormat() failed. Invalid set file pointer.");

		// Locate the 'RIFF' chunk in the audio file, and check the file type.
		DWORD dwChunkSize;
		DWORD dwChunkPosition;
		//check the file type, should be fourccWAVE or 'XWMA'
		findChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
		DWORD filetype;
		readChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
		ErrorIfTrue(filetype != fourccWAVE, L"AudioSample::loadFormat() failed. File type is not wave.");

		// Locate the 'fmt ' chunk, and copy its contents into a WAVEFORMATEXTENSIBLE structure.
		findChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
		readChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	}

	void AudioSample::unload(void)
	{
		if (buffer.pAudioData != nullptr)
		{
			delete buffer.pAudioData;
			buffer.pAudioData = nullptr;
		}
	}

	HRESULT AudioSample::findChunk(HANDLE file, DWORD fourcc, DWORD& chunkSize, DWORD& chunkDataPosition)
	{
		HRESULT hr = S_OK;
		ErrorIfTrue(INVALID_SET_FILE_POINTER == SetFilePointer(file, 0, NULL, FILE_BEGIN), L"AudioSample::findChunk() failed. Invalid set file pointer."); //return HRESULT_FROM_WIN32(GetLastError());
		DWORD dwChunkType;
		DWORD dwChunkDataSize;
		DWORD dwRIFFDataSize = 0;
		DWORD dwFileType;
		DWORD bytesRead = 0;
		DWORD dwOffset = 0;
		while (hr == S_OK)
		{
			DWORD dwRead;
			ErrorIfTrue(0 == ReadFile(file, &dwChunkType, sizeof(DWORD), &dwRead, NULL), L"AudioSample::findChunk() failed. Unable to read file."); //hr = HRESULT_FROM_WIN32(GetLastError());

			ErrorIfTrue(0 == ReadFile(file, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL), L"AudioSample::findChunk() failed. Unable to read file."); //hr = HRESULT_FROM_WIN32(GetLastError());

			switch (dwChunkType)
			{
			case fourccRIFF:
				dwRIFFDataSize = dwChunkDataSize;
				dwChunkDataSize = 4;
				ErrorIfTrue(0 == ReadFile(file, &dwFileType, sizeof(DWORD), &dwRead, NULL), L"AudioSample::findChunk() failed. Unable to read file."); //hr = HRESULT_FROM_WIN32(GetLastError());
				break;
			default:
				ErrorIfTrue(INVALID_SET_FILE_POINTER == SetFilePointer(file, dwChunkDataSize, NULL, FILE_CURRENT), L"AudioSample::findChunk() failed. Invalid set file pointer."); //return HRESULT_FROM_WIN32(GetLastError());
			}

			dwOffset += sizeof(DWORD) * 2;

			if (dwChunkType == fourcc)
			{
				chunkSize = dwChunkDataSize;
				chunkDataPosition = dwOffset;
				return S_OK;
			}

			dwOffset += dwChunkDataSize;
			ErrorIfTrue(bytesRead >= dwRIFFDataSize, L"AudioSample::findChunk() failed. bytesRead >= dwRIFFDataSize."); //return S_FALSE;
		}
		return S_OK;
	}

	HRESULT AudioSample::readChunkData(HANDLE file, void* buffer, DWORD buffersize, DWORD bufferoffset)
	{
		HRESULT hr = S_OK;
		ErrorIfTrue(INVALID_SET_FILE_POINTER == SetFilePointer(file, bufferoffset, NULL, FILE_BEGIN), L"AudioSample::readChunkData() failed. Invalid set file pointer.");//return HRESULT_FROM_WIN32(GetLastError());
		DWORD dwRead;
		ErrorIfTrue(0 == ReadFile(file, buffer, buffersize, &dwRead, NULL), L"AudioSample::readChunkData() failed. Unable to read file."); //hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}
}