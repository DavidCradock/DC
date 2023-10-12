/* Main include files for all DC code.
*/
#pragma once
#include "AI/AI.h"
#include "Audio/Audio.h"
#include "Common/common.h"
#include "Image/image.h"
#include "Input/input.h"
#include "Math/math.h"
#include "Physics/physics.h"
#include "Renderer/renderer.h"
#include "SpatialPartitioning/spatialPartitioning.h"
#include "UserInterface/userInterface.h"

#ifdef _DEBUG
#pragma comment(lib, "../../../x64/Debug/DavesCode.lib")
#else
#pragma comment(lib, "../../../x64/Release/DavesCode.lib")
#endif