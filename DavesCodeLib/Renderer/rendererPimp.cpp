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

		initCreateSwapchain();
		initCreateImageViews();
		initSetupDepthStencil();
		initCreateRenderPass();
		initCreateFramebuffers();

		initCreateCommandPool();
		initCreateCommandBuffers();
		initCreateSemaphores();
		initCreateFences();
	}

	void Renderer::Pimpl::shutdown(void)
	{
		log.addEntryINFO(L"Renderer::Pimple::shutdown() called.");

		log.addEntryNOSTAT(L"Destroying fences.");
		for (auto inFlightFence : fences)
		{
			vkDestroyFence(device, inFlightFence, nullptr);
		}

		log.addEntryNOSTAT(L"Destroying \"image available\" semaphore.");
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);

		log.addEntryNOSTAT(L"Destroying \"rendering finished\" semaphore.");
		vkDestroySemaphore(device, renderingFinishedSemaphore, nullptr);
		
		log.addEntryNOSTAT(L"Destroying command pool.");
		vkDestroyCommandPool(device, commandPool, nullptr);

		log.addEntryNOSTAT(L"Destroying framebuffers.");
		for (auto framebuffer : swapchainFramebuffers)
		{
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		log.addEntryNOSTAT(L"Destroying render pass.");
		vkDestroyRenderPass(device, renderPass, nullptr);

		log.addEntryNOSTAT(L"Destroying objects for depth stencil.");
		vkDestroyImageView(device, depthImageView, nullptr);
		vkDestroyImage(device, depthImage, nullptr);
		vkFreeMemory(device, depthImageMemory, nullptr);

		log.addEntryNOSTAT(L"Destroying swapchain image views.");
		for (auto imageView : swapchainImageViews)
		{
			vkDestroyImageView(device, imageView, nullptr);
		}

		log.addEntryNOSTAT(L"Destroying swapchain.");
		vkDestroySwapchainKHR(device, swapchain, nullptr);

		log.addEntryNOSTAT(L"Destroying device.");
		vkDestroyDevice(device, nullptr);

		if (debugMessengerNeedsShutdown)
		{
			log.addEntryNOSTAT(L"Destroying the debug utilities messenger.");
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr)
				func(instance, debugMessenger, nullptr);
		}

		log.addEntryNOSTAT(L"Destroying surface.");
		vkDestroySurfaceKHR(instance, surface, nullptr);

		log.addEntryNOSTAT(L"Destroying instance.");
		vkDestroyInstance(instance, nullptr);

		log.addEntryNOSTAT(L"Destroying window.");
		SDL_DestroyWindow(SDLWindow);

		log.addEntryNOSTAT(L"Calling SDL_Quit().");
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
		createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		// Previous implementations of Vulkan made a distinction between instance and device specific validation layers,
		// but this is no longer the case.That means that the enabledLayerCount and ppEnabledLayerNames fields of VkDeviceCreateInfo
		// are ignored by up to date implementations.
		// However, it is still a good idea to set them anyway to be compatible with older implementations.
		createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();

		if (VK_SUCCESS != vkCreateDevice(physicalDevice, &createInfo, nullptr, &device))
		{
			ErrorIfTrue(true, L"Error creating logical Vulkan device.");
		}

		vkGetDeviceQueue(device, queueFamilyIndexGraphics, 0, &graphicsQueue);
		vkGetDeviceQueue(device, queueFamilyIndexPresent, 0, &presentQueue);

		ErrorIfFalse(graphicsQueue, L"Error whilst calling vkGetDeviceQueue() for the graphics queue.");
		ErrorIfFalse(presentQueue, L"Error whilst calling vkGetDeviceQueue() for the presentation queue.");
		log.addEntryPASS(L"Logical device has been created and the graphics and presentation queues have been obtained.");
	}

	void Renderer::Pimpl::initCreateSwapchain(void)
	{
		log.addEntryINFO(L"Renderer::Pimpl::initCreateSwapchain() called.");

		// Get surface capabilities and log the important ones
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
		log.addEntryINFO(L"Listing surface capabilities...");
		String logEntryText = L"currentExtent: Width: ";
		logEntryText.appendInt(surfaceCapabilities.currentExtent.width);
		logEntryText.append(L" Height: ");
		logEntryText.appendInt(surfaceCapabilities.currentExtent.height);
		log.addEntryNOSTAT(logEntryText);

		// Get the surface formats
		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		uint32_t surfaceFormatsCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatsCount,	nullptr);
		surfaceFormats.resize(surfaceFormatsCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatsCount,	surfaceFormats.data());
		
		// Attempt to find the surface format which is VK_FORMAT_B8G8R8A8_SRGB and VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, otherwise bail out
		bool bFoundAcceptableFormat = false;
		for (const auto& availableFormat : surfaceFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				bFoundAcceptableFormat = true;
				surfaceFormat = availableFormat;
				break;
			}
		}
		ErrorIfFalse(bFoundAcceptableFormat, L"Unable to find required surface format of VK_FORMAT_B8G8R8A8_SRGB and colourspace VK_COLOR_SPACE_SRGB_NONLINEAR_KHR");

		// Get size of swapchain from SDL
		int width, height = 0;
		SDL_Vulkan_GetDrawableSize(SDLWindow, &width, &height);
		swapchainSize.width = (uint32_t)width;
		swapchainSize.height = (uint32_t)height;
		if (swapchainSize.width < surfaceCapabilities.minImageExtent.width)
			swapchainSize.width = surfaceCapabilities.minImageExtent.width;
		if (swapchainSize.width > surfaceCapabilities.maxImageExtent.width)
			swapchainSize.width = surfaceCapabilities.maxImageExtent.width;
		if (swapchainSize.height < surfaceCapabilities.minImageExtent.height)
			swapchainSize.height = surfaceCapabilities.minImageExtent.height;
		if (swapchainSize.height > surfaceCapabilities.maxImageExtent.height)
			swapchainSize.height = surfaceCapabilities.maxImageExtent.height;
		logEntryText = L"Swapchain dimensions: ";
		logEntryText.appendInt((int)swapchainSize.width);
		logEntryText.append(L"x");
		logEntryText.appendInt((int)swapchainSize.height);
		log.addEntryNOSTAT(logEntryText);

		// Decide how many images to have.
		// If we stick to the minimum supported, we may have to wait for the GPU to finish doing it's thing before we can acquire the next image to render to,
		// so it's recommended to ask for the minimum supported + 1.
		uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
		// if maxImageCount == 0, then there's no limit.
		// Make sure we keep within the max limit...
		if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
		{
			imageCount = surfaceCapabilities.maxImageCount;
		}
		logEntryText = L"Minimum number of images the physical device supports for a swapchain created for the surface: ";
		logEntryText.appendInt(surfaceCapabilities.minImageCount);
		log.addEntryNOSTAT(logEntryText);
		logEntryText = L"Maximum number of images the physical device supports for a swapchain created for the surface: ";
		if (surfaceCapabilities.maxImageCount == 0)
			logEntryText.append(L" No limit");
		else
			logEntryText.appendInt(surfaceCapabilities.maxImageCount);
		log.addEntryNOSTAT(logEntryText);
		logEntryText = L"Number of chosen images to get: ";
		logEntryText.appendInt((int)imageCount);
		log.addEntryNOSTAT(logEntryText);
		// Make sure the number of images if valid.
		if (imageCount != surfaceCapabilities.minImageCount + 1)
		{
			ErrorIfTrue(1, L"We need minimum number of images + 1, sorry.");
		}

		// Now create the swapchain
		log.addEntryINFO(L"Attempting to create the swapchain...");
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = surfaceCapabilities.minImageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = swapchainSize;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = { queueFamilyIndexGraphics, queueFamilyIndexPresent };
		log.addEntryNOSTAT(L"Checking whether both queue family indicies for presentation and graphics are equal or not...");
		if (queueFamilyIndexGraphics != queueFamilyIndexPresent)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
			log.addEntryWARN(L"Both queue family indicies are not the same. Setting sharing mode to concurrent. (Sub-optimal performance)");
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			log.addEntryPASS(L"Both queue family indicies are the same. Setting sharing mode to exclusive. (Optimal performance)");
		}

		createInfo.preTransform = surfaceCapabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		createInfo.clipped = VK_TRUE;

		log.addEntryNOSTAT(L"Calling vkCreateSwapchain()...");
		if (VK_SUCCESS != vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain))
		{
			ErrorIfTrue(true, L"Error whilst attempting to create the swapchain.");
		}

		log.addEntryPASS(L"Swapchain successfully created.");
		
		log.addEntryNOSTAT(L"Obtaining number of image handles within the swapchain...");
		if (VK_SUCCESS != vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr))
		{
			ErrorIfTrue(true, L"Error during call to vkGetSwapchainImagesKHR()");
		}

		log.addEntryPASS(L"Number of images retrieved.");

		swapchainImages.resize(swapchainImageCount);

		log.addEntryNOSTAT(L"Obtaining image handles within the swapchain...");
		if (VK_SUCCESS != vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data()))
		{
			ErrorIfTrue(true, L"Error during call to vkGetSwapchainImagesKHR()");
		}
		log.addEntryPASS(L"Image handles successfully retrieved.");
	}

	void Renderer::Pimpl::initCreateImageViews(void)
	{
		swapchainImageViews.resize(swapchainImages.size());

		log.addEntryINFO(L"initCreateImageViews() called.");

		for (uint32_t i = 0; i < swapchainImages.size(); i++)
		{
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = swapchainImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = surfaceFormat.format;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (VK_SUCCESS != vkCreateImageView(device, &viewInfo, nullptr, &swapchainImageViews[i]))
			{
				ErrorIfTrue(true, L"Error whilst creating image view.");
			}
		}

		String logEntryText = L"All ";
		logEntryText.appendInt((int)swapchainImages.size());
		logEntryText.append(L" image views have been created.");
		log.addEntryPASS(logEntryText);
	}

	void Renderer::Pimpl::initSetupDepthStencil(void)
	{
		// Get supported depth format
		log.addEntryINFO(L"Renderer::Pimpl::initSetupDepthStencil() called.");

		std::vector<VkFormat> depthFormats = {
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
		};

		bool bFoundDepthFormat = false;
		for (auto& format : depthFormats)
		{
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
			if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				depthFormat = format;
				bFoundDepthFormat = true;
				break;
			}
		}
		ErrorIfFalse(bFoundDepthFormat, L"Error whilst trying to obtain depth format for depth stencil.");
		log.addEntryPASS(L"Depth format for depth stencil found.");

		// Attempt to create image for depth stencil
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = swapchainSize.width;
		imageInfo.extent.height = swapchainSize.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(device, &imageInfo, nullptr, &depthImage) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, depthImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;

		bool bFoundMemoryTypeIndex = false;
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			{
				allocInfo.memoryTypeIndex = i;
				bFoundMemoryTypeIndex = true;
				break;
			}
		}
		ErrorIfFalse(bFoundMemoryTypeIndex, L"Unable to find memory type index.");
		
		if (vkAllocateMemory(device, &allocInfo, nullptr, &depthImageMemory) != VK_SUCCESS)
		{
			ErrorIfTrue(1, L"Failed to allocate image memory.");
		}

		vkBindImageMemory(device, depthImage, depthImageMemory, 0);

		// Create image view
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = depthImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (VK_SUCCESS != vkCreateImageView(device, &viewInfo, nullptr, &depthImageView))
		{
			ErrorIfTrue(1, L"Error whilst attempting to create image view for depth stencil.");
		}
	}

	void Renderer::Pimpl::initCreateRenderPass(void)
	{
		log.addEntryINFO(L"Renderer::Pimpl::initCreateRenderPass() called...");

		std::vector<VkAttachmentDescription> attachments(2);

		attachments[0].format = surfaceFormat.format;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		attachments[1].format = depthFormat;
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorReference;
		subpassDescription.pDepthStencilAttachment = &depthReference;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;
		subpassDescription.pResolveAttachments = nullptr;

		std::vector<VkSubpassDependency> dependencies(1);

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		if (VK_SUCCESS != vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass))
		{
			ErrorIfTrue(1, L"Error during call to vkCreateRenderPass()");
		}
		log.addEntryPASS(L"Successfully created render pass.");
	}

	void Renderer::Pimpl::initCreateFramebuffers(void)
	{
		log.addEntryINFO(L"Renderer::Pimpl::initCreateFramebuffers() called.");

		swapchainFramebuffers.resize(swapchainImageViews.size());

		for (size_t i = 0; i < swapchainImageViews.size(); i++)
		{
			std::vector<VkImageView> attachments(2);
			attachments[0] = swapchainImageViews[i];
			attachments[1] = depthImageView;

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapchainSize.width;
			framebufferInfo.height = swapchainSize.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS)
			{
				ErrorIfTrue(1, L"Error whilst creating framebuffers");
			}
		}
		log.addEntryPASS(L"Creation of framebuffers successfull.");
	}

	void Renderer::Pimpl::initCreateCommandPool(void)
	{
		log.addEntryINFO(L"Renderer::Pimpl::initCreateCommandPool() called.");

		VkCommandPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		createInfo.queueFamilyIndex = queueFamilyIndexGraphics;
		VkResult result = vkCreateCommandPool(device, &createInfo, nullptr, &commandPool);
		if (VK_SUCCESS != result)
		{
			ErrorIfTrue(1, L"Error whilst creating command pool.");
		}
		log.addEntryPASS(L"Command pool created.");
	}

	void Renderer::Pimpl::initCreateCommandBuffers(void)
	{
		log.addEntryINFO(L"Renderer::Pimpl::initCreateCommandBuffers() called.");

		VkCommandBufferAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = commandPool;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = swapchainImageCount;

		commandBuffers.resize(swapchainImageCount);
		VkResult result = vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data());
		if (VK_SUCCESS != result)
		{
			ErrorIfTrue(1, L"Error whilst creating command buffers.");
		}
		log.addEntryPASS(L"Command buffers created.");
	}

	void Renderer::Pimpl::initCreateSemaphores(void)
	{
		log.addEntryINFO(L"Renderer::Pimpl::initCreateSemaphores() called.");

		VkSemaphoreCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VkResult result = vkCreateSemaphore(device, &createInfo, nullptr, &imageAvailableSemaphore);
		if (VK_SUCCESS != result)
		{
			ErrorIfTrue(1, L"Error whilst creating \"image available\" semaphore.");
		}
		result = vkCreateSemaphore(device, &createInfo, nullptr, &renderingFinishedSemaphore);
		if (VK_SUCCESS != result)
		{
			ErrorIfTrue(1, L"Error whilst creating \"rendering finished\" semaphore.");
		}
		log.addEntryPASS(L"Both semaphores created. (Rendering finished and image available)");
	}

	void Renderer::Pimpl::initCreateFences(void)
	{
		log.addEntryINFO(L"Renderer::Pimpl::initCreateFences() called.");

		uint32_t i;
		fences.resize(swapchainImageCount);
		for (i = 0; i < swapchainImageCount; i++)
		{
			VkFenceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			VkResult result = vkCreateFence(device, &createInfo, nullptr, &fences[i]);
			if (VK_SUCCESS != result)
			{
				ErrorIfTrue(1, L"Error whilst creating fence.");
			}
		}
		log.addEntryPASS(L"Fences successfully created.");
	}

}