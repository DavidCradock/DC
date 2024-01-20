#pragma once
#include <SDL.h>
#include <SDL_vulkan.h>
#include "../Common/error.h"
#include "renderer.h"
#include <vulkan/vulkan.h>

namespace DC
{
	// Private implementation class which holds all Vulkan objects etc and has various methods which we keep in here
	// instead of in the Renderer class, to keep that class nice and tidy.
	class Renderer::Pimpl
	{
	public:
		Pimpl();

		// Called from Renderer::init()
		// Calls all this class's private init???? methods.
		// The order of their calling is important. Obviously, we can see the order within this method.
		void init(const Settings& settings);

		// Called from Renderer::update()
		// Returns false if the window/application has been asked to close/shutdown
		bool update(void);

		// Called from Renderer::shutdown()
		void shutdown(void);

	private:
		// Called from init to initialise the application's window as well as the SDL library itself.
		// Call order is important for all init???? methods, see init() for order.
		void initWindow(const Settings& settings);

		// The SDL, Vulkan enabled window handle.
		SDL_Window* SDLWindow;

		// Called from init to initialise the main Vulkan instance
		// Call order is important for all init???? methods, see init() for order.
		void initVulkanInstance(const Settings& settings);

		// The main Vulkan instance
		VkInstance instance;

		// Called from init to initialise the Vulkan validation layer callback if settings is set to use them.
		void initVulkanValidationLayer(const Settings& settings);

		// Vulkan debug messenger handle used to deal with the debug callback
		VkDebugUtilsMessengerEXT debugMessenger;

		// Set in initVulkanValidationLayer so that shutdown() can determine whether to cleanup the debugMessenger or not.
		bool debugMessengerNeedsShutdown;

		// Debug callback for validation layers
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
	};


}