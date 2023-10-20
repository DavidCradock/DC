#pragma once
#include "rendererBase.h"

namespace DC
{
	class RendererOpenGL : public Renderer
	{
	public:
		RendererOpenGL();
		~RendererOpenGL();

		void init(void);
		void shutdown(void);

		void blendDisable(void);
		void blendEnable(void);
		void blendFunction_SrcAlpha_One(void);
		void blendFunction_SrcAlpha_OneMinusSrcAlpha(void);

		void cullFaceAntiClockwise(void);
		void cullFaceClockwise(void);
		void cullFaceDisable(void);
		void cullFaceEnable(void);

		void depthTestDisable(void);
		void depthTestEnable(void);

		void pixelsRead(int positionX, int positionY, int width, int height, void* pixels);
		void pixelsWrite(int positionX, int positionY, int width, int height, void* pixels);

		void scissorTest(int positionX, int positionY, int width, int height);
		void scissorTestDisable(void);
		void scissorTestEnable(void);

		// Add a new, uniquely named vertex buffer object.
		// If the named object already exists, this simply returns the object's pointer
		// If locked is set to true, this affects the various remove method's behaviours.
		// If calling a remove method on a named object which is set as locked, it will not
		// be removed unless that remove method is passed true to it's forceRemoveLocked parameter.
		// Objects can later be unlocked with a call to setLocked().
		VertexBufferBase* addVertexBuffer(const String& uniqueName, bool locked);
	private:
		class impl;
		impl* pimp;
	};
}