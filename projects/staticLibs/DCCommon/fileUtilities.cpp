#include "fileUtilities.h"

namespace DC
{
	bool getFileExists(const String& strFilename)
	{
		/*
		FILE* f;
		if (fopen_s(&f, strFilename.c_str(), "rb"))
			return false;
		fclose(f);
		return true;
		*/
		bool bExists = false;
		std::ifstream file(strFilename);
		if (file.is_open())
		{
			bExists = true;
			file.close();
		}
		return bExists;
	}

	bool deleteFile(const String& strFilenameToDelete)
	{
		std::remove(strFilenameToDelete.c_strn());
		return !getFileExists(strFilenameToDelete);
	}

	bool renameFile(const String& strOldFilename, const String& strNewFilename)
	{
		if (0 == std::rename(strOldFilename.c_strn(), strNewFilename.c_strn()))
			return true;
		return false;
	}
}