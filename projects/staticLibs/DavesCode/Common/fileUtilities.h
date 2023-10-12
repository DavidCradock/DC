#pragma once
#include "string.h"
#include "error.h"

namespace DC
{
	// Helper method. Returns true if the given filename exists on storage device
	bool getFileExists(const String& strFilename);

	// Read a type from an already opened ifstream in binary mode.
	// data This will hold the data being read from the stream.
	// If the given ifstream is not opened or an error occurred, an exception occurs
	// returns false if an error occurred.
	template <class Type> void read(std::ifstream& file, Type& data);

	// Write a type to an already opened ofstream in binary mode.
	// data This is what will be written to the ofstream.
	// If the given ofstream is not opened or an error occurred, an exception occurs
	// returns false if an error occurred.
	template <class Type> void write(std::ofstream& file, Type& data);

	// Attempts to delete the given file name.
	// If the file still exists after attempting to delete it, this method returns false, else true
	bool deleteFile(const String& strFilenameToDelete);

	// Attempts to rename the given file name to the new filename.
	// Returns whether the operation was a success or not
	bool renameFile(const String& strOldFilename, const String& strNewFilename);

	template<class Type> void read(std::ifstream& file, Type& data)
	{
		ErrorIfFalse(file.is_open(), L"read() failed. The given ifstream is not open.");
		ErrorIfTrue(file.bad(), L"read() failed. The given ifstream is bad.");

		size_t stTypeSize = sizeof(Type);
		file.read((char*)&data, stTypeSize);
		ErrorIfTrue(file.bad(), L"read() failed. The given ifstream is bad.");
	}

	template<class Type> void write(std::ofstream& file, Type& data)
	{
		ErrorIfFalse(file.is_open(), L"write() failed. The given ofstream is not open.");
		ErrorIfTrue(file.bad(), L"write() failed. The given ofstream is bad.");

		size_t stTypeSize = sizeof(Type);
		file.write((char*)&data, stTypeSize);
		ErrorIfTrue(file.bad(), L"write() failed. The given ofstream is bad.");
	}
}