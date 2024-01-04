#include "renderer.h"
#include "../Common/error.h"
#include "Helpers/vkError.h"
#include <vulkan/vulkan.h>
#include <map>

/*
Vulkan main objects and their use.

	VkInstance : The Vulkan context, used to access drivers.
	VkPhysicalDevice : A GPU. Used to query physical GPU details, like features, capabilities, memory size, etc.
	VkDevice : The “logical” GPU context that you actually execute things on.
	VkBuffer : A chunk of GPU visible memory.
	VkImage : A texture you can write to and read from.
	VkPipeline : Holds the state of the gpu needed to draw. For example: shaders, rasterization options, depth settings.
	VkRenderPass : Holds information about the images you are rendering into. All drawing commands have to be done inside a render pass.
	VkFrameBuffer : Holds the target images for a render pass.
	VkCommandBuffer : Encodes GPU commands. All execution that is performed on the GPU itself (not in the driver) has to be encoded in a VkCommandBuffer.
	VkQueue : Execution “port” for commands. GPUs will have a set of queues with different properties. Some allow only graphics commands, others only allow memory commands, etc. Command buffers are executed by submitting them into a queue, which will copy the rendering commands onto the GPU for execution.
	VkDescriptorSet : Holds the binding information that connects shader inputs to data such as VkBuffer resources and VkImage textures. Think of it as a set of gpu-side pointers that you bind once.
	VkSwapchainKHR : Holds the images for the screen. It allows you to render things into a visible window. The KHR suffix shows that it comes from an extension, which in this case is VK_KHR_swapchain
	VkSemaphore : Synchronizes GPU to GPU execution of commands. Used for syncing multiple command buffer submissions one after other.
	VkFence : Synchronizes GPU to CPU execution of commands. Used to know if a command buffer has finished being executed on the GPU.


Engine initialization

First, everything is initialized. To initialize Vulkan, you start by creating a VkInstance. From the VkInstance, you query the list of VkPhysicalDevice handles available in the machine.
For example, if the computer had both a dedicated GPU and integrated graphics, there would be a VkPhysicalDevice for each. After querying the limits and features of the available
VkPhysicalDevice handles, you create a VkDevice from it. With a VkDevice, you then get VkQueue handles from it, allowing you to execute commands. Then you initialize the VkSwapchainKHR.
Alongside the VkQueue handles, you create VkCommandPool objects that enable you to allocate command buffers from them.


Asset initialization

Once the core structures are initialized, you initialize the resources you need for whatever you will be rendering. The materials are loaded, and you create a set of VkPipeline objects
for the shader combinations and parameters needed to render the materials. For the meshes, you upload their vertex data into VkBuffer resources, and you upload their textures into
VkImage resources, making sure that the images are in “readable” layout. You also create VkRenderPass objects for whatever your main rendering passes you have. For example, there may
be a VkRenderPass for the main rendering, and another for a shadow pass. On a real engine, much of this can be parallelized and done in background threads, especially since pipeline
creation can be quite expensive.


Render Loop

Now that everything is ready for rendering, you first ask the VkSwapchainKHR for an image to render to. Then you allocate a VkCommandBuffer from a VkCommandBufferPool or reuse an
already allocated command buffer that has finished execution, and “start” the command buffer, which allows you to write commands into it. Next, you begin rendering by starting a
VkRenderPass, which specifies that you are rendering to the image requested from swapchain, then create a loop where you bind a VkPipeline, bind some VkDescriptorSet resources
(for the shader parameters), bind the vertex buffers, and then execute a draw call. Once you are finished with the drawing for a pass, you end the VkRenderPass. If there is nothing
more to render, you end the VkCommandBuffer. Finally, you submit the command buffer into the queue for rendering. This will begin execution of the commands in the command buffer on
the gpu. If you want to display the result of the rendering, you “present” the image you have rendered to to the screen. Because the execution may not have finished yet, you use a
semaphore to make the presentation of the image to the screen wait until rendering is finished.
*/
/*
namespace DC
{

	// Private implementation class which holds all Vulkan objects etc and has various methods which we keep in here
	// instead of in the Renderer class, to keep that class nice and tidy.
	class Renderer::Pimpl
	{
	public:
		Pimpl();

		// GLFW Window pointer
		GLFWwindow* glfwWindow;

		// Vulkan instance.
		// The Vulkan context, used to access drivers.
		// It's the first Vulkan object that is created.
		VkInstance vulkanInstance;

		// Vulkan debug messenger handle used to deal with the debug callback
		VkDebugUtilsMessengerEXT debugMessenger;

		// Vulkan window surface which we render to
		VkSurfaceKHR surface;

		// Vulkan GPU physical device representing the choosen GPU on the system
		// which supports our requirements.
		VkPhysicalDevice physicalDevice;

		// Vulkan logical device.
		// This is what we execute things on.
		VkDevice device;

		// Vulkan logical device graphics queue.
		// No need to destroy as they're cleaned up when the logical device (device) is destroyed.
		VkQueue graphicsQueue;

		// Vulkan presentation queue handle
		VkQueue presentQueue;

		// Main initialization
		void init(const Settings& settings);

		// Shutdown everything
		void shutdown(void);

		// Debug callback
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
	};

	Renderer::Pimpl::Pimpl()
	{
		glfwWindow = 0;
		physicalDevice = VK_NULL_HANDLE;
	}

	void Renderer::Pimpl::init(const Settings& settings)
	{

		// Create GLFW window

		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindow = glfwCreateWindow(settings.getWindowWidthWhenWindowed(), settings.getWindowHeightWhenWindowed(), "DC", nullptr, nullptr);
		ErrorIfFalse(glfwWindow, L"Unable to create application window.");


		// Create Vulkan instance

		// Use validation layers for debug builds
#ifdef NDEBUG
		bool enableValidationLayers = false;
#else
		bool enableValidationLayers = true;
#endif

		// Get the extensions which GLFW requires
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		// Copy the extensions to a vector and add the VK_EXT_DEBUG_UTILS_EXTENSION_NAME extension to that if we're in debug build
		std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		if (enableValidationLayers)
			requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		// Get the extensions count and properties, which exist
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// Make sure the required extensions which we need exist
		uint32_t extensionsFound = 0;
		for (uint32_t i = 0; i < requiredExtensions.size(); i++)
		{
			for (uint32_t j = 0; j < extensionCount; j++)
			{
				if (strcmp(extensions[j].extensionName, requiredExtensions[i]) == 0)
				{
					extensionsFound++;
					break;
				}
			}
		}
		ErrorIfFalse(extensionsFound == requiredExtensions.size(), L"Required extensions not found.");

		// Here we set which layers we require.
		// VK_LAYER_KHRONOS_validation is the validation layer which is available when the Vulkan SDK is installed.
		const std::vector<const char*> requestedValidationLayers = { "VK_LAYER_KHRONOS_validation" };

		// Get number of available layers and their properties
		uint32_t availableLayerCount;
		vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(availableLayerCount);
		vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

		// If enableValidationLayers is true, then we check that all requested layers are present
		if (enableValidationLayers)
		{
			int layersFound = 0;
			for (uint32_t i = 0; i < requestedValidationLayers.size(); i++)
			{
				for (uint32_t j = 0; j < availableLayerCount; j++)
				{
					if (strcmp(requestedValidationLayers[i], availableLayers[j].layerName) == 0)
					{
						layersFound++;
						break;
					}
				}
			}
			if (layersFound != requestedValidationLayers.size())
			{
				ErrorIfTrue(1, L"We're in debug build and the requested number of layers could not be found.");
			}

			// Make sure the debug extensions exist
			bool requiredDebugUtilExtensionExists = false;
			for (uint32_t j = 0; j < extensionCount; j++)
			{
				if (strcmp(extensions[j].extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
				{
					requiredDebugUtilExtensionExists = true;
					break;
				}
			}
			ErrorIfFalse(requiredDebugUtilExtensionExists, L"Required extension VK_EXT_DEBUG_UTILS_EXTENSION_NAME does not exist.");
		}

		// Fill in the technically optional application information struct so that drivers can optimize and such for our application.
		// We'll use this when filling in the Vulkan instanceCreateInfo struct below
		VkApplicationInfo appInfo{};
		appInfo.apiVersion = VK_API_VERSION_1_0;
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pApplicationName = "DC";
		appInfo.pEngineName = "DC Engine";
		appInfo.pNext = nullptr;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

		// Setup createInfo structure
		VkInstanceCreateInfo createInfoVKInstance{};
		createInfoVKInstance.enabledExtensionCount = (uint32_t)requiredExtensions.size();
		// Fill in if validation layers are required
		if (enableValidationLayers)
		{
			createInfoVKInstance.enabledLayerCount = static_cast<uint32_t>(requestedValidationLayers.size());
			createInfoVKInstance.ppEnabledLayerNames = requestedValidationLayers.data();
		}
		else
		{
			createInfoVKInstance.enabledLayerCount = 0;
		}
		createInfoVKInstance.pApplicationInfo = &appInfo;
		createInfoVKInstance.ppEnabledExtensionNames = requiredExtensions.data();
		createInfoVKInstance.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		VkResult result = vkCreateInstance(&createInfoVKInstance, nullptr, &vulkanInstance);
		vkError(result);


		// Create debug callback

		// Only setup in debug builds
		if (enableValidationLayers)
		{
			VkDebugUtilsMessengerCreateInfoEXT createInfoDebugUtilsMessenger{};
			createInfoDebugUtilsMessenger.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			// All types except for VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT here to receive notifications about possible problems while leaving out verbose general debug info.
			createInfoDebugUtilsMessenger.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfoDebugUtilsMessenger.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfoDebugUtilsMessenger.pfnUserCallback = debugCallback;
			createInfoDebugUtilsMessenger.pUserData = nullptr; // Optional

			// The above struct should be passed to the vkCreateDebugUtilsMessengerEXT function.
			// But, because this function is an extension function, it is not automatically loaded. We have to look up its address ourselves using vkGetInstanceProcAddr
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugUtilsMessengerEXT");
			ErrorIfTrue(func == nullptr, L"VK_ERROR vkCreateDebugUtilsMessengerEXT not present.");

			VkResult result = func(vulkanInstance, &createInfoDebugUtilsMessenger, nullptr, &debugMessenger);
			ErrorIfTrue(result != VK_SUCCESS, L"Error creating debug utils messenger.");
		}


		// Create Vulkan surface

		ErrorIfTrue(glfwCreateWindowSurface(vulkanInstance, glfwWindow, nullptr, &surface) != VK_SUCCESS, L"Failed to create window surface.");


		// Select GPU physical device

		// Get number of GPU devices
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);
		ErrorIfFalse(deviceCount, L"Unable to find any Vulkan compatible GPU devices on this system.");

		// Get each GPU device
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, devices.data());

		// Use an ordered map to automatically sort candidates by increasing score
		std::multimap<int, VkPhysicalDevice> candidates;

		// For each device
		for (const auto& device : devices)
		{
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


		// Create Vulkan device

		// Get number of queue families of the chosen physical device
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		// Get each queues' properties
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		// Find index to queue family of the physical device which has the VK_QUEUE_GRAPHICS_BIT flag as well as
		// having the ability to present to the window surface.
		int i = 0;
		bool bQueueFamilyFound = false;
		int iIndexToQueueFamily = -1;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
				if (presentSupport)
				{
					iIndexToQueueFamily = i;
					bQueueFamilyFound = true;
				}
			}
			i++;
		}
		ErrorIfFalse(bQueueFamilyFound, L"Unable to find a queue family which has VK_QUEUE_GRAPHICS_BIT and supports presenting to the window surface.");

		// Setup queue creation information struct
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = iIndexToQueueFamily;
		queueCreateInfo.queueCount = 1;

		// Vulkan lets you assign priorities to queues to influence the scheduling of command buffer execution using floating point numbers between 0.0 and 1.0.
		// This is required even if there is only a single queue
		float fQueuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &fQueuePriority;

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfoDevice{};
		createInfoDevice.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfoDevice.pQueueCreateInfos = &queueCreateInfo;
		createInfoDevice.queueCreateInfoCount = 1;
		createInfoDevice.pEnabledFeatures = &deviceFeatures;
		// Previous implementations of Vulkan made a distinction between instance and device specific validation layers,
		// but this is no longer the case.That means that the enabledLayerCount and ppEnabledLayerNames fields of VkDeviceCreateInfo
		// are ignored by up to date implementations.
		// However, it is still a good idea to set them anyway to be compatible with older implementations.
		createInfoDevice.enabledExtensionCount = 0;
		if (enableValidationLayers)
		{
			createInfoDevice.enabledLayerCount = static_cast<uint32_t>(requestedValidationLayers.size());
			createInfoDevice.ppEnabledLayerNames = requestedValidationLayers.data();
		}
		else
		{
			createInfoDevice.enabledLayerCount = 0;
		}
		// Create the logical device
		if (vkCreateDevice(physicalDevice, &createInfoDevice, nullptr, &device) != VK_SUCCESS)
		{
			ErrorIfTrue(1, L"Failed to create the Vulkan logical device.");
		}

		// Get handle to the logical device's graphics queue
		vkGetDeviceQueue(device, iIndexToQueueFamily, 0, &graphicsQueue);



		// Create presentation queue

		VkDeviceQueueCreateInfo queuePresentationCreateInfo{};
		queuePresentationCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queuePresentationCreateInfo.queueFamilyIndex = iIndexToQueueFamily;
		queuePresentationCreateInfo.queueCount = 1;
		queuePresentationCreateInfo.pQueuePriorities = &fQueuePriority;
		vkGetDeviceQueue(device, iIndexToQueueFamily, 0, &presentQueue);
		ErrorIfFalse(presentQueue, L"Unable to obtain presentation queue.");


		// Swapchain setup

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

	}



	void Renderer::Pimpl::shutdown(void)
	{
		// Logical Vulkan device
		vkDestroyDevice(device, nullptr);


		// Debug callback

		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(vulkanInstance, debugMessenger, nullptr);


		// Vulkan surface and instance

		vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);
		vkDestroyInstance(vulkanInstance, nullptr);


		// GLFW

		glfwDestroyWindow(glfwWindow);
		glfwTerminate();
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

	Renderer::Renderer()
	{
		pimp = new Renderer::Pimpl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	Renderer::~Renderer()
	{
		delete pimp;
		pimp = 0;
	}

	void Renderer::init(const Settings& settings)
	{
		pimp->init(settings);
	}

	void Renderer::shutdown(void)
	{
		pimp->shutdown();
	}

	bool Renderer::update(void)
	{
		bool bQuit = false;
		glfwPollEvents();
		if (glfwWindowShouldClose(pimp->glfwWindow))
			bQuit = true;
		return !bQuit;
	}
}
*/