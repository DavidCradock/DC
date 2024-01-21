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
		// Call order is important for all init???? methods, see init() for order.
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

		// Called from init to create the window surface using SDL.
		// This has to be created right after the Vulkan instance as it can affect physical device selection.
		// Call order is important for all init???? methods, see init() for order.
		void initCreateWindowSurface(void);

		// The window's surface object, created by SDL.
		VkSurfaceKHR surface;

		// Called from init to select the correct physical device.
		// We select the device which is discreet and supports a minimum of 16k image sizes
		// Call order is important for all init???? methods, see init() for order.
		void initSelectPhysicalDevice(void);

		// The Vulkan GPU physical device we select to use from initSelectPhysicalDevice()
		// This doesn't need to be destroyed as it is when the vkInstance is.
		VkPhysicalDevice physicalDevice;

		// Called from init to select the two queue family indicies which support graphics and presentation
		// Call order is important for all init???? methods, see init() for order.
		void initSelectQueueFamilyIndicies(void);

		// The queue family index which supports graphics
		uint32_t queueFamilyIndexGraphics;

		// The queue familt index which supports presentation to the window surface
		uint32_t queueFamilyIndexPresent;
		
		// Called from init to create the two queues and the logical Vulkan device
		// Call order is important for all init???? methods, see init() for order.
		void initCreateLogicalDevice(const Settings& settings);

		// The logical Vulkan device created from the physical GPU device.
		VkDevice device;

		// The graphics queue
		VkQueue graphicsQueue;

		// The presentation queue
		VkQueue presentQueue;
	};


}