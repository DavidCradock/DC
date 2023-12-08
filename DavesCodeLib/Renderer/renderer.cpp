#include "renderer.h"
#include "../Common/error.h"
#include "vkError.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

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
namespace DC
{
	class Renderer::Pimpl
	{
	public:
		Pimpl();

		// GLFW Window pointer
		GLFWwindow* glfwWindow;

		// Vulkan instance.
		// The Vulkan context, used to access drivers.
		// It's the first thing in Vulkan that's created.
		VkInstance vulkanInstance;

		// Create the Vulkan instance
		void createVulkanInstance(void);

		// Cleanup Vulkan objects
		void cleanupVulkan(void);
	};

	Renderer::Pimpl::Pimpl()
	{
		glfwWindow = 0;
	}

	void Renderer::Pimpl::createVulkanInstance(void)
	{
		VkApplicationInfo appInfo{};
		appInfo.apiVersion = VK_API_VERSION_1_0;
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.pEngineName = "No Engine";
		appInfo.pNext = nullptr;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

		// Get required extensions by glfw
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		// Get number of extensions which exist and their names
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// Make sure required extensions exist
		uint32_t extensionsFound = 0;
		for (uint32_t i = 0; i < glfwExtensionCount; i++)
		{
			for (uint32_t j = 0; j < extensionCount; j++)
			{
				if (strcmp(extensions[j].extensionName, glfwExtensions[i]) == 0)
				{
					extensionsFound++;
					break;
				}
			}
		}
		ErrorIfFalse(extensionsFound == glfwExtensionCount, L"Required extensions not found.");

		VkInstanceCreateInfo createInfo{};
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.enabledLayerCount = 0;
//		createInfo.flags;
		createInfo.pApplicationInfo = &appInfo;
//		createInfo.pNext;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
//		createInfo.ppEnabledLayerNames;
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;



		VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkanInstance);
		vkError(result);
	}

	void Renderer::Pimpl::cleanupVulkan(void)
	{
		vkDestroyInstance(vulkanInstance, nullptr);
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
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		pimp->glfwWindow = glfwCreateWindow(settings.getWindowWidthWhenWindowed(), settings.getWindowHeightWhenWindowed(), "Vulkan window", nullptr, nullptr);
		ErrorIfFalse(pimp->glfwWindow, L"Unable to create application window.");

		pimp->createVulkanInstance();

		
		
	}

	void Renderer::shutdown(void)
	{
		pimp->cleanupVulkan();

		glfwDestroyWindow(pimp->glfwWindow);
		glfwTerminate();
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