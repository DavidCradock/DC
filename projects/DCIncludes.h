/* Main include files for all DC code.
	Also links all static libraries.
*/
#pragma once
#include "../../staticLibs/DCAI/DCAI.h"
#include "../../staticLibs/DCAudio/DCAudio.h"
#include "../../staticLibs/DCCommon/DCCommon.h"
#include "../../staticLibs/DCImage/DCImage.h"
#include "../../staticLibs/DCInput/DCInput.h"
#include "../../staticLibs/DCMath//DCMath.h"
#include "../../staticLibs/DCPhysics/DCPhysics.h"
#include "../../staticLibs/DCRenderer/DCRenderer.h"
#include "../../staticLibs/DCSpatialPartitioning/DCSpatialPartitioning.h"
#include "../../staticLibs/DCUI/DCUI.h"
#ifdef _DEBUG
#pragma comment(lib, "../../../x64/Debug/DCAI.lib")
#pragma comment(lib, "../../../x64/Debug/DCAudio.lib")
#pragma comment(lib, "../../../x64/Debug/DCCommon.lib")
#pragma comment(lib, "../../../x64/Debug/DCImage.lib")
#pragma comment(lib, "../../../x64/Debug/DCInput.lib")
#pragma comment(lib, "../../../x64/Debug/DCMath.lib")
#pragma comment(lib, "../../../x64/Debug/DCPhysics.lib")
#pragma comment(lib, "../../../x64/Debug/DCRenderer.lib")
#pragma comment(lib, "../../../x64/Debug/DCSpatialPartitioning.lib")
#pragma comment(lib, "../../../x64/Debug/DCUI.lib")
#else
#pragma comment(lib, "../../../x64/Release/DCAI.lib")
#pragma comment(lib, "../../../x64/Release/DCAudio.lib")
#pragma comment(lib, "../../../x64/Release/DCCommon.lib")
#pragma comment(lib, "../../../x64/Release/DCImage.lib")
#pragma comment(lib, "../../../x64/Release/DCInput.lib")
#pragma comment(lib, "../../../x64/Release/DCMath.lib")
#pragma comment(lib, "../../../x64/Release/DCPhysics.lib")
#pragma comment(lib, "../../../x64/Release/DCRenderer.lib")
#pragma comment(lib, "../../../x64/Release/DCSpatialPartitioning.lib")
#pragma comment(lib, "../../../x64/Release/DCUI.lib")
#endif