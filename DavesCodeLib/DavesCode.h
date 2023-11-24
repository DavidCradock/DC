/* Main include files for all DC code.
*/
#pragma once
#include "AI/AI.h"
#include "Audio/Audio.h"
#include "Common/common.h"
#include "Engine/engine.h"
#include "Image/image.h"
#include "Input/input.h"
#include "Math/math.h"
#include "Physics/physics.h"
#include "Renderer/renderer.h"
#include "SpatialPartitioning/spatialPartitioning.h"
#include "UserInterface/userInterface.h"

#ifdef _DEBUG
#pragma comment(lib, "../_build/DavesCodeLib/OutputDir/x64/Debug/DavesCodeLib.lib")
#else
#pragma comment(lib, "../_build/DavesCodeLib/OutputDir/x64/Release/DavesCodeLib.lib")
#endif

// Third party libs

