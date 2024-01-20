#include "rendererPimp.h"
#include "../Common/logging.h"

namespace DC
{
	Renderer::Pimpl::Pimpl()
	{
	}

	void Renderer::Pimpl::init(const Settings& settings)
	{
		// Initialise SDL
		SDL_Init(SDL_INIT_EVERYTHING);

		initWindow(settings);
		initVulkanInstance(settings);
		initVulkanValidationLayer(settings);
	}

	void Renderer::Pimpl::shutdown(void)
	{
		if (debugMessengerNeedsShutdown)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr)
				func(instance, debugMessenger, nullptr);
		}

		vkDestroyInstance(instance, nullptr);
		SDL_DestroyWindow(SDLWindow);
		SDL_Quit();
	}

	void Renderer::Pimpl::initWindow(const Settings& settings)
	{
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

		SDLWindow = SDL_CreateWindow(
			"Dave's Code Library Application.",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			settings.getWindowWidthWhenWindowed(),
			settings.getWindowHeightWhenWindowed(),
			window_flags
		);
		if (!SDLWindow)
		{
			String error = L"SDL error occurred whilst attempting to create application window. The SDL error message is...\n";
			error.appendCharPointer(SDL_GetError());
			ErrorIfTrue(1, error);
		}
	}

	void Renderer::Pimpl::initVulkanInstance(const Settings& settings)
	{
		log.addEntrySeperator();
		log.addEntryINFO(L"initVulkanInstance() called.");

		// Get supported extensions
		uint32_t supportedExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
		std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

		// Log supported extensions
		String logEntryText = L"Listing supported extensions (";
		logEntryText.appendInt(supportedExtensionCount);
		logEntryText.append(L")...");
		log.addEntryINFO(logEntryText);
		for (size_t i = 0; i < supportedExtensions.size(); i++)
		{
			String strExtension;
			strExtension.appendCharPointer(supportedExtensions[i].extensionName);
			strExtension.append(L" Spec Version: ");
			strExtension.appendInt(supportedExtensions[i].specVersion);
			log.addEntryNOSTAT(strExtension);
		}

		// Get required extensions by SDL (VK_KHR_SURFACE and VK_KHR_win32_surface)
		unsigned int requiredExtensionCount = 0;
		SDL_Vulkan_GetInstanceExtensions(SDLWindow, &requiredExtensionCount, nullptr);
		std::vector<const char*> requiredExtensionNames(requiredExtensionCount);
		SDL_Vulkan_GetInstanceExtensions(SDLWindow, &requiredExtensionCount, requiredExtensionNames.data());

		// Add the validation layers extension as required if set in the settings object
		if (settings.getVulkanValidationLayersUsage())
		{
			requiredExtensionCount++;
			requiredExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		// Log required extensions
		logEntryText = L"Listing required extensions (";
		logEntryText.appendUnsignedInt(requiredExtensionCount);
		logEntryText.append(L")...");
		log.addEntryINFO(logEntryText);
		for (unsigned int i = 0; i < requiredExtensionCount; i++)
		{
			logEntryText = L"Extension #";
			logEntryText.appendUnsignedInt(i);
			logEntryText.append(L" ");
			logEntryText.appendCharPointer(requiredExtensionNames[i]);
			log.addEntryNOSTAT(logEntryText);
		}
		
		// Make sure the required extensions which we need exist
		uint32_t extensionsFound = 0;
		for (uint32_t i = 0; i < requiredExtensionNames.size(); i++)
		{
			for (uint32_t j = 0; j < supportedExtensions.size(); j++)
			{
				if (strcmp(supportedExtensions[j].extensionName, requiredExtensionNames[i]) == 0)
				{
					extensionsFound++;
					break;
				}
			}
		}
		if (extensionsFound != requiredExtensionNames.size())	// Not all required extensions found
		{
			if (settings.getVulkanValidationLayersUsage())	// If Vulkan validation layers are set in settings file
			{
				logEntryText.clear();
				logEntryText.append(L"Not all required Vulkan extensions were found.\n");
				logEntryText.appendCharPointer(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				logEntryText.append(L" extension requested due to useVulkanValidationLayers being true in settings file.\n");
				logEntryText.append(L"Make sure the VulkanSDK is installed, or change useVulkanValidationLayers to be false in the settings file.");
			}
			else
			{
				logEntryText.clear();
				logEntryText.append(L"Not all required Vulkan extensions were found.");
			}
			ErrorIfTrue(1, logEntryText);
		}

		// Create instance
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = settings.getApplicationName().c_strn();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Fill vector with validation layer names if needed before adding to vkInstanceCreateInfo struct below.
		// VK_LAYER_KHRONOS_validation is the validation layer which is available when the Vulkan SDK is installed.
		std::vector<const char*> validationLayers;
		const char* newString = "VK_LAYER_KHRONOS_validation";
		if (settings.getVulkanValidationLayersUsage())
		{
			validationLayers.push_back(newString);
		}

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledLayerCount = (uint32_t)validationLayers.size();
		instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		instanceCreateInfo.enabledExtensionCount = (uint32_t)requiredExtensionNames.size();
		instanceCreateInfo.ppEnabledExtensionNames = requiredExtensionNames.data();

		VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
		ErrorIfTrue(result != VK_SUCCESS, L"Unable to create Vulkan instance object.");
		log.addEntryPASS(L"Vulkan instance created.");
	}

	bool Renderer::Pimpl::update(void)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				return false;
			}
		}

		// Return false if window has been asked to close.
		return true;
	}

	void Renderer::Pimpl::initVulkanValidationLayer(const Settings& settings)
	{
		debugMessengerNeedsShutdown = false;

		// Only setup in debug builds
		if (!settings.getVulkanValidationLayersUsage())
			return;

		debugMessengerNeedsShutdown = true;

		VkDebugUtilsMessengerCreateInfoEXT createInfoDebugUtilsMessenger{};
		createInfoDebugUtilsMessenger.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		// All types except for VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT here to receive notifications about possible problems while leaving out verbose general debug info.
		createInfoDebugUtilsMessenger.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfoDebugUtilsMessenger.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfoDebugUtilsMessenger.pfnUserCallback = debugCallback;
		createInfoDebugUtilsMessenger.pUserData = nullptr; // Optional

		// The above struct should be passed to the vkCreateDebugUtilsMessengerEXT function.
		// But, because this function is an extension function, it is not automatically loaded. We have to look up its address ourselves using vkGetInstanceProcAddr
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		ErrorIfTrue(func == nullptr, L"VK_ERROR vkCreateDebugUtilsMessengerEXT not present.");

		VkResult result = func(instance, &createInfoDebugUtilsMessenger, nullptr, &debugMessenger);
		ErrorIfTrue(result != VK_SUCCESS, L"Error creating debug utils messenger.");
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::Pimpl::debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		// messageSeverity can be one of the following...
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: Diagnostic message
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT : Informational message like the creation of a resource
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT : Message about behaviour that is not necessarily an error, but very likely a bug in your application
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT : Message about behaviour that is invalid and may cause crashes

		// messageType parameter can be one of the following...
		// VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: Some event has happened that is unrelated to the specification or performance
		// VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT : Something has happened that violates the specification or indicates a possible mistake
		// VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT : Potential non - optimal use of Vulkan

		// pCallbackData parameter refers to a VkDebugUtilsMessengerCallbackDataEXT struct containing the details of the message itself, with the most important members being:
		// pMessage: The debug message as a null - terminated string
		// pObjects : Array of Vulkan object handles related to the message
		// objectCount : Number of objects in array

		// pUserData parameter contains a pointer that was specified during the setup of the callback and allows you to pass your own data to it.

		String strError;
		strError.appendCharPointer(pCallbackData->pMessage);
		ErrorIfTrue(1, strError);

		return VK_FALSE;
	}


}