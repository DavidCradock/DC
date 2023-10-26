#pragma once
#include "fragmentProgramManager.h"
#include "texture2DManager.h"
#include "vertexBufferManager.h"
#include "vertexProgramManager.h"

namespace DC
{
	// Vulkan renderer.
	// Creates and returns a graphics renderer used for creating a window, hooking
	// up the Vulkan graphics API to it and performing rendering operations in that window.
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		// Creates a window and hooks up the Vulkan graphics API to it, ready for rendering
		void init(void);

		// Closes the window and cleans everything up
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

		// The fragment program manager.
		FragmentProgramManager fragmentProgramManager;

		// The texture2D manager.
		Texture2DManager texture2DManager;

		// The vertex buffer manager.
		VertexBufferManager vertexBufferManager;

		// The vertex program manager.
		VertexProgramManager vertexProgramManager;
	private:
		class impl;
		impl* pimp;
	};
}