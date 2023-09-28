#include "error.h"
#include <Windows.h>

namespace DC
{
	void ErrorFunc(const String& errorDescription, const String& errorSourceFileName, const String& errorSourceFileLine)
	{
		String error = errorDescription;
		error += "\nSource file name: " + errorSourceFileName;
		error += "\nSource file line: " + errorSourceFileLine;
		std::wstring wstring = error.wideGet();
		MessageBoxW(NULL, wstring.c_str(), L"I'm truely sorry. A critical error has occurred...", MB_OK);
	}

	void (*errorFunc)(const String& errorDescription, const String& errorSourceFileName, const String& errorSourceFileLine) = ErrorFunc;
}