#pragma once
#include "../Common/error.h"

namespace DC
{
#ifndef vkError
#define vkError(x)\
		{\
		if(x){\
		std::string strvkError = x;\
		DC::String strErrorWide;\
		strErrorWide.multiByteToWideChar(strvkError);\
		ErrorIfTrue(1, )\
		}
#endif
}