#pragma once
#include "string.h"


	// Function pointer which gets called when a critical error occurs.
	// By default, this points to a function which appends each of the given strings into a single string and shows that in a message box.
	// To create an alternate function, create one with the same signature as below and point errorFunc to the new function.
	extern void (*errorFunc)(const std::wstring& errorDescription, const std::wstring& errorSourceFileName, const std::wstring& errorSourceFileLine);

#ifndef _DEBUG
	// Macro to check a value x and if false, calls errorFunc passing the strings containing error description, filename and line number.
	#ifndef ErrorIfFalse
	#define ErrorIfFalse(x, strErrorDescription)\
		{\
		if(!x){\
		std::string strFilename = __FILE__;\
		DC::String strFilenameWide;\
		strFilenameWide.multiByteToWideChar(strFilename);\
		DC::String strLineNumber;\
		strLineNumber.appendInt(__LINE__);\
		errorFunc(strErrorDescription, strFilenameWide, strLineNumber);}\
		}
	#endif

	// Macro to check a value x and if true, calls errorFunc passing the strings containing error description, filename and line number.
	#ifndef ErrorIfTrue
	#define ErrorIfTrue(x, strErrorDescription)\
		{\
		if(x){\
		std::string strFilename = __FILE__;\
		DC::String strFilenameWide;\
		strFilenameWide.multiByteToWideChar(strFilename);\
		DC::String strLineNumber;\
		strLineNumber.appendInt(__LINE__);\
		errorFunc(strErrorDescription, strFilenameWide, strLineNumber);}\
		}
	#endif

	// Macro to check a value x and if zero, calls errorFunc passing "Memory allocation error." as the error description, filename and line number.
	#ifndef ErrorIfMemoryNotAllocated
	#define ErrorIfMemoryNotAllocated(x)\
		{\
		if(!x){\
		std::string strFilename = __FILE__;\
		DC::String strFilenameWide;\
		strFilenameWide.multiByteToWideChar(strFilename);\
		DC::String strLineNumber;\
		strLineNumber.appendInt(__LINE__);\
		errorFunc(L"Memory allocation error.", strFilenameWide, strLineNumber);}\
		}
	#endif

#else
	// Macro to check a value x and if false, calls errorFunc passing the given error description String y, filename and line number as Strings.
	#ifndef ErrorIfFalse
	#define ErrorIfFalse(x, strErrorDescription)\
		{\
		if(!x){\
		__debugbreak();\
		std::string strFilename = __FILE__;\
		DC::String strFilenameWide;\
		strFilenameWide.multiByteToWideChar(strFilename);\
		DC::String strLineNumber;\
		strLineNumber.appendInt(__LINE__);\
		errorFunc(strErrorDescription, strFilenameWide, strLineNumber);}\
		}
	#endif

	// Macro to check a value x and if true, calls errorFunc passing the given error description String y, filename and line number as Strings.
	#ifndef ErrorIfTrue
	#define ErrorIfTrue(x, strErrorDescription)\
		{\
		if(x){\
		__debugbreak();\
		std::string strFilename = __FILE__;\
		DC::String strFilenameWide;\
		strFilenameWide.multiByteToWideChar(strFilename);\
		DC::String strLineNumber;\
		strLineNumber.appendInt(__LINE__);\
		errorFunc(strErrorDescription, strFilenameWide, strLineNumber);}\
		}
	#endif

	// Macro to check a value x and if zero, calls errorFunc passing "Memory allocation error." as the error description, filename and line number as Strings.
	#ifndef ErrorIfMemoryNotAllocated
	#define ErrorIfMemoryNotAllocated(x)\
		{\
		if(!x){\
		__debugbreak();\
		std::string strFilename = __FILE__;\
		DC::String strFilenameWide;\
		strFilenameWide.multiByteToWideChar(strFilename);\
		DC::String strLineNumber;\
		strLineNumber.appendInt(__LINE__);\
		errorFunc(L"Memory allocation error.", strFilenameWide, strLineNumber);}\
		}
	#endif
#endif
