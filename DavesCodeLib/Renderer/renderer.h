#pragma once
#include "../Engine/settings.h"
#include <vulkan/vulkan.h>

namespace DC
{
	// Vulkan renderer.
	// A graphics renderer used for creating a window, hooking
	// up the Vulkan graphics API to it and performing rendering operations in that window.
	// The application engine has an object of this class. See Engine/engine.h
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		// Creates a window and hooks up the Vulkan graphics API to it, ready for rendering
		void init(const Settings &settings);

		// Closes the window and cleans everything up
		void shutdown(void);

		// Called each program loop updating the window etc.
		// Returns false if the window has been asked to close or alt+F4 has been pressed
		bool update(void);

	private:


	};
}