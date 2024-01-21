#include "rendererPimp.h"
#include "../Common/logging.h"
#include <map>
#include <set>

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
		initCreateWindowSurface();
		initSelectPhysicalDevice();
		initSelectQueueFamilyIndicies();
		initCreateLogicalDevice(settings);

	}

	void Renderer::Pimpl::shutdown(void)
	{

		vkDestroyDevice(device, nullptr);

		if (debugMessengerNeedsShutdown)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr)
				func(instance, debugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(instance, surface, nullptr);
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
		// This code is used in initVulkanInstance AND initCreateLogicalDevice
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

	void Renderer::Pimpl::initCreateWindowSurface(void)
	{
		log.addEntryINFO(L"initCreateWindowSurface() called...");
		ErrorIfFalse(SDL_TRUE == SDL_Vulkan_CreateSurface(SDLWindow, instance, &surface), L"Error whilst creating window surface with SDL.");
		log.addEntryPASS(L"Surface successfully created by SDL.");
	}

	void Renderer::Pimpl::initSelectPhysicalDevice(void)
	{
		log.addEntryINFO(L"initSelectPhysicalDevice() called...");

		// Get number of GPU devices
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		ErrorIfFalse(deviceCount, L"Unable to find any Vulkan compatible GPU devices on this system.");

		// Log number of GPU devices found
		String logEntryText;
		logEntryText.append(L"Number of GPU devices found: ");
		logEntryText.appendInt(deviceCount);
		log.addEntryINFO(logEntryText);

		// Get each GPU device
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		// Use an ordered map to automatically sort candidates by increasing score
		std::multimap<int, VkPhysicalDevice> candidates;

		// For each device
		int iDeviceNumber = 0;	// For logging text
		for (const auto& device : devices)
		{
			iDeviceNumber++;

			// Get device properties
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);

			// Get device features
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			// Give score based on certain features
			int deviceScore = 0;
			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				deviceScore += 1000;
			if (deviceProperties.limits.maxImageDimension2D < 16384)
				deviceScore = 0;
			if (!deviceFeatures.geometryShader)
				deviceScore = 0;
			if (!deviceFeatures.tessellationShader)
				deviceScore = 0;
			candidates.insert(std::make_pair(deviceScore, device));

			// Log out information about the physical device
			log.addEntryNOSTAT(L"Physical device properties...");
			logEntryText = L"Device number: ";	logEntryText.appendInt(iDeviceNumber);	logEntryText.append(L" of ");	logEntryText.appendInt(deviceCount);	log.addEntryNOSTAT(logEntryText);
			logEntryText = L"apiVersion: ";	logEntryText.appendInt(deviceProperties.apiVersion);	log.addEntryNOSTAT(logEntryText);
			logEntryText = L"deviceName: ";	logEntryText.appendCharPointer(deviceProperties.deviceName);	log.addEntryNOSTAT(logEntryText);
			logEntryText = L"deviceType: ";
			if (deviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU)
				logEntryText.append(L"CPU");
			else if (deviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				logEntryText.append(L"DISCRETE GPU");
			else if (deviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				logEntryText.append(L"INTEGRATED GPU");
			else if (deviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM)
				logEntryText.append(L"MAX ENUM");
			else if (deviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_OTHER)
				logEntryText.append(L"OTHER");
			else if (deviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
				logEntryText.append(L"VIRTUAL GPU");
			else
				logEntryText.append(L"UNKNOWN");
			log.addEntryNOSTAT(logEntryText);
			logEntryText = L"driverVersion: ";	logEntryText.appendInt(deviceProperties.driverVersion);	log.addEntryNOSTAT(logEntryText);
			logEntryText = L"maxImageDimensions2D: ";	logEntryText.appendInt(deviceProperties.limits.maxImageDimension2D);	log.addEntryNOSTAT(logEntryText);
			log.addEntryNOSTAT(L"Physical device features...");
			logEntryText = L"geometryShader: "; logEntryText.appendInt((int)deviceFeatures.geometryShader); log.addEntryNOSTAT(logEntryText);
			logEntryText = L"tessellationShader: "; logEntryText.appendInt((int)deviceFeatures.tessellationShader); log.addEntryNOSTAT(logEntryText);
		}

		// Set physical device to use the one given the highest score above
		physicalDevice = VK_NULL_HANDLE;
		if (candidates.rbegin()->first > 0)
		{
			physicalDevice = candidates.rbegin()->second;
		}
		else
		{
			ErrorIfTrue(1, L"Failed to find a suitable GPU. We need a device which supports geometry and tesselation shaders and has a minimum 2D image size of 16384.\nPerhaps try updating your drivers?");
		}
	}

	void Renderer::Pimpl::initSelectQueueFamilyIndicies(void)
	{
		log.addEntryINFO(L"initSelectQueueFamilyIndicies() called.");

		// Get number and properties of queue families
		std::vector<VkQueueFamilyProperties> queueFamilyProperties;
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		queueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

		// Log number of queue families found.
		String logEntryText = L"Number of queue families found: ";	logEntryText.appendInt(queueFamilyCount);	log.addEntryNOSTAT(logEntryText);

		// Go through each of the families until we've found a queue family index for both a queue family which supports graphics and presentation.
		int graphicIndex = -1;
		int presentIndex = -1;
		int i = 0;
		for (const auto& queueFamily : queueFamilyProperties)
		{
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicIndex = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
			if (queueFamily.queueCount > 0 && presentSupport)
			{
				presentIndex = i;
			}

			if (graphicIndex != -1 && presentIndex != -1)
			{
				break;
			}

			i++;
		}

		queueFamilyIndexGraphics = graphicIndex;
		queueFamilyIndexPresent = presentIndex;

		ErrorIfTrue(queueFamilyIndexGraphics == -1, L"Unable to find an index to a queue family which supports the graphics bit set.");
		ErrorIfTrue(queueFamilyIndexPresent == -1, L"Unable to find an index to a queue family which supports presentation to the window surface.");
		logEntryText = L"Queue family indicies found for graphics(";
		logEntryText.appendInt(queueFamilyIndexGraphics);
		logEntryText.append(L") and presentation(");
		logEntryText.appendInt(queueFamilyIndexPresent);
		logEntryText.append(L")");
		log.addEntryPASS(logEntryText);
	}

	void Renderer::Pimpl::initCreateLogicalDevice(const Settings& settings)
	{
		log.addEntryINFO(L"Renderer::initCreateLogicalDevice() called...");

		// Fill vector with validation layer names if needed before adding to vkInstanceCreateInfo struct below.
		// VK_LAYER_KHRONOS_validation is the validation layer which is available when the Vulkan SDK is installed.
		// This code is used in initVulkanInstance AND initCreateLogicalDevice
		std::vector<const char*> validationLayers;
		const char* newString = "VK_LAYER_KHRONOS_validation";
		if (settings.getVulkanValidationLayersUsage())
		{
			validationLayers.push_back(newString);
		}

		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		// Vulkan lets us assign priorities to queues to influence the scheduling of command buffer execution using floating point numbers between 0.0 and 1.0.
		// This is required even if there is only a single queue
		const float queue_priority[] = { 1.0f };

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { queueFamilyIndexGraphics, queueFamilyIndexPresent };

		float queuePriority = queue_priority[0];
		for (int queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndexGraphics;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		//https://en.wikipedia.org/wiki/Anisotropic_filtering
		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = queueCreateInfos.size();
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = deviceExtensions.size();
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		// Previous implementations of Vulkan made a distinction between instance and device specific validation layers,
		// but this is no longer the case.That means that the enabledLayerCount and ppEnabledLayerNames fields of VkDeviceCreateInfo
		// are ignored by up to date implementations.
		// However, it is still a good idea to set them anyway to be compatible with older implementations.
		createInfo.enabledLayerCount = validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();

		ErrorIfFalse(VK_SUCCESS == vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), L"Error creating logical Vulkan device.");

		vkGetDeviceQueue(device, queueFamilyIndexGraphics, 0, &graphicsQueue);
		vkGetDeviceQueue(device, queueFamilyIndexPresent, 0, &presentQueue);

		ErrorIfFalse(graphicsQueue, L"Error whilst calling vkGetDeviceQueue() for the graphics queue.");
		ErrorIfFalse(presentQueue, L"Error whilst calling vkGetDeviceQueue() for the presentation queue.");
		log.addEntryPASS(L"Logical device has been created and the graphics and presentation queues have been obtained.");
	}
}