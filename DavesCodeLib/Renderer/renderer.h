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
	// The application framework has an object of this class. See Framework/framework.h
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		// Creates a window and hooks up the Vulkan graphics API to it, ready for rendering
		void init(void);

		// Closes the window and cleans everything up
		void shutdown(void);

		// Called each program loop updating the window etc.
		// Returns false if the window has been asked to close or alt+F4 has been pressed
		bool update(void);

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