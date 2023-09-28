#pragma once
#include "string.h"

namespace DC
{
	// Function pointer which gets called when a critical error occurs.
	// By default, this points to a function which appends each of the given strings into a single string and shows that in a messagebox.
	// To create an alternate function, create one with the same signature as below and call errorSetFunc() to set.
	void (*errorFunc)(const String& errorDescription, const String& errorSourceFileName, const String& errorSourceFileLine);

#ifndef _DEBUG
	// Macro to check a value x and if false, calls errorFunc passing the given error description String y, filename and line number as Strings.
	#ifndef ErrorIfFalse
	#define ErrorIfFalse(x, y)						\
		{											\
		if(!x){										\
		errorFunc(y, __FILE__, __LINE__);}			\
		}
	#endif

	// Macro to check a value x and if true, calls errorFunc passing the given error description String y, filename and line number as Strings.
	#ifndef ErrorIfTrue
	#define ErrorIfTrue(x, y)						\
		{											\
		if(x){										\
		errorFunc(y, __FILE__, __LINE__);}			\
		}
	#endif

	// Macro to check a value x and if zero, calls errorFunc passing "Memory allocation error." as the error description, filename and line number as Strings.
	#ifndef ErrorIfMemoryNotAllocated
	#define ErrorIfMemoryNotAllocated(x)			\
		{											\
		if(!x){										\
		errorFunc("Memory allocation error.", __FILE__, __LINE__);}			\
		}
	#endif

#else
	// Macro to check a value x and if false, calls errorFunc passing the given error description String y, filename and line number as Strings.
	#ifndef ErrorIfFalse
	#define ErrorIfFalse(x, y)						\
		{											\
		if(!x){										\
 		__debugbreak();								\
		errorFunc(y, __FILE__, __LINE__);}			\
		}
	#endif

	// Macro to check a value x and if true, calls errorFunc passing the given error description String y, filename and line number as Strings.
	#ifndef ErrorIfTrue
	#define ErrorIfTrue(x, y)						\
		{											\
		if(x){										\
 		__debugbreak();								\
		errorFunc(y, __FILE__, __LINE__);}			\
		}
	#endif

	// Macro to check a value x and if zero, calls errorFunc passing "Memory allocation error." as the error description, filename and line number as Strings.
	#ifndef ErrorIfMemoryNotAllocated
	#define ErrorIfMemoryNotAllocated(x)			\
		{											\
		if(!x){										\
 		__debugbreak();								\
		errorFunc("Memory allocation error.", __FILE__, __LINE__);}			\
		}
	#endif
#endif
}