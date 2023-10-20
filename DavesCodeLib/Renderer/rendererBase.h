#pragma once
#include "../Common/string.h"
#include "fragmentProgramBase.h"
#include "texture2DBase.h"
#include "vertexBufferBase.h"
#include "vertexProgramBase.h"

namespace DC
{
	// Base class which declares which methods need to be in a renderer class.
	// No implementation here.
	// The renderer class should contain everything needed to use a graphics API.
	// This includes creation/destruction/usage of the window, textures, vertex buffers and shader programs.
	class Renderer
	{
	public:
		// Should create the window and setup the API ready for use.
		virtual void init(void) = 0;

		// Should free everything, shutdown and close the window.
		virtual void shutdown(void) = 0;

		virtual void blendDisable(void) = 0;
		virtual void blendEnable(void) = 0;
		virtual void blendFunction_SrcAlpha_One(void) = 0;
		virtual void blendFunction_SrcAlpha_OneMinusSrcAlpha(void) = 0;
		
		virtual void cullFaceAntiClockwise(void) = 0;
		virtual void cullFaceClockwise(void) = 0;
		virtual void cullFaceDisable(void) = 0;
		virtual void cullFaceEnable(void) = 0;

		virtual void depthTestDisable(void) = 0;
		virtual void depthTestEnable(void) = 0;
		
		virtual void pixelsRead(int positionX, int positionY, int width, int height, void* pixels) = 0;
		virtual void pixelsWrite(int positionX, int positionY, int width, int height, void* pixels) = 0;

		virtual void scissorTest(int positionX, int positionY, int width, int height) = 0;
		virtual void scissorTestDisable(void) = 0;
		virtual void scissorTestEnable(void) = 0;

		// Add a new, uniquely named vertex buffer object.
		// If the named object already exists, this simply returns the object's pointer
		// If locked is set to true, this affects the various remove method's behaviours.
		// If calling a remove method on a named object which is set as locked, it will not
		// be removed unless that remove method is passed true to it's forceRemoveLocked parameter.
		// Objects can later be unlocked with a call to setLocked().
		virtual VertexBufferBase* addVertexBuffer(const String& uniqueName, bool locked) = 0;
	};
}