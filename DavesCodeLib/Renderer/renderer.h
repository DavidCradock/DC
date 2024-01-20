#pragma once
#include "../Engine/settings.h"

namespace DC
{
	// Vulkan renderer.
	// A graphics renderer used for creating a window, hooking
	// up the Vulkan graphics API to it and performing rendering operations in that window.
	// The application engine has an object of this class. See Engine/engine.h
	// This uses SDL. It initialises all of SDL, creates a Vulkan/SDL window and sets up Vulkan with it's debugging stuff, all ready for use.
	// In Settings, if useVulkanValidationLayers is true, Vulkan's validation layers and debugging is attempted to be setup, regardless of whether
	// the Vulkan SDK is installed. If it isn't, an error will occur. Either set the settings to false, or install the Vulkan SDK.
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
		class Pimpl;	// Private implementation holding stuff which we do not wish to expose when including this header file.
		Pimpl* pimp;	// Pointer holding private implementation.

	};
}