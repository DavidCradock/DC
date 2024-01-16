#include "error.h"
#include <Windows.h>
#include "logging.h"
//#include "string.h"

void ErrorFunc(const std::wstring& errorDescription, const std::wstring& errorSourceFileName, const std::wstring& errorSourceFileLine)
{
	std::wstring error;
	error += L"Source file name:\n" + errorSourceFileName;
	error += L"\n\nSource file line:\n" + errorSourceFileLine;
	error += L"\n\nError Description:\n" + errorDescription;
	
	// Add log entry
	DC::String logEntry;
	logEntry.append(error);
	DC::log.addEntryFAIL(logEntry);

	MessageBoxW(NULL, error.c_str(), L"I'm sorry. A critical error has occurred...", MB_OK);
}
void (*errorFunc)(const std::wstring& errorDescription, const std::wstring& errorSourceFileName, const std::wstring& errorSourceFileLine) = ErrorFunc;
