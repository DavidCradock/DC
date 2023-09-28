#include "error.h"
#include <Windows.h>

namespace DC
{
	void ErrorFunc(const String& errorDescription, const String& errorSourceFileName, const String& errorSourceFileLine)
	{
		String error = errorDescription;
//		error += L"\nSource file name: " + errorSourceFileName;
//		error += L"\nSource file line: " + errorSourceFileLine;
		MessageBoxW(NULL, error.c_str(), L"I'm truely sorry. A critical error has occurred ): :( ...", MB_OK);
	}

	extern void (*errorFunc)(const String& errorDescription, const String& errorSourceFileName, const String& errorSourceFileLine) = ErrorFunc;
}