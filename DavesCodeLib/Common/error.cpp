#include "error.h"
#include <Windows.h>

void ErrorFunc(const std::wstring& errorDescription, const std::wstring& errorSourceFileName, const std::wstring& errorSourceFileLine)
{
	std::wstring error;
	error += L"Source file name:\n" + errorSourceFileName;
	error += L"\n\nSource file line:\n" + errorSourceFileLine;
	error += L"\n\nError Description:\n" + errorDescription;
	MessageBoxW(NULL, error.c_str(), L"I'm sorry. A critical error has occurred...", MB_OK);
}
void (*errorFunc)(const std::wstring& errorDescription, const std::wstring& errorSourceFileName, const std::wstring& errorSourceFileLine) = ErrorFunc;
