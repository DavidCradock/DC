#include "error.h"
#include <Windows.h>

namespace DC
{
	void ErrorFunc(const String& errorDescription, const String& errorSourceFileName, const String& errorSourceFileLine)
	{
		String error;
		error += String(L"Source file name:\n") + errorSourceFileName;
		error += String(L"\n\nSource file line:\n") + errorSourceFileLine;
		error += String(L"\n\nError Desciption:\n") + errorDescription;
		MessageBoxW(NULL, error.c_str(), L"I'm sorry. A critical error has occurred...", MB_OK);
	}

	void (*errorFunc)(const String& errorDescription, const String& errorSourceFileName, const String& errorSourceFileLine) = ErrorFunc;
}