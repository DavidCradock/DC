#include "renderer.h"
#include "../Common/error.h"
#include "vkError.h"
#include "../../third-party/vk-bootstrap-main/src/VkBootstrap.h"

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
	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::init(const Settings& settings)
	{
		windowExtent.width = settings.getWindowWidthWhenWindowed();
		windowExtent.height = settings.getWindowHeightWhenWindowed();

		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
		SDL_window = SDL_CreateWindow(
			"Dave's Code: Vulkan Engine",
			SDL_WINDOWPOS_UNDEFINED, // Window position x (don't care)
			SDL_WINDOWPOS_UNDEFINED, // Window position y (don't care)
			windowExtent.width, // Window width in pixels
			windowExtent.height, // Window height in pixels
			window_flags
		);

		// Create Vulkan instance
		vkb::InstanceBuilder builder;
		// Make the Vulkan instance, with basic debug features
		auto inst_ret = builder.set_app_name("Example Vulkan Application")
			.request_validation_layers(true)
			.require_api_version(1, 1, 0)
			.use_default_debug_messenger()
			.build();

		vkb::Instance vkb_inst = inst_ret.value();

		// Store the instance
		vkInstance = vkb_inst.instance;

		// Store the debug messenger
		vkDebugMessenger = vkb_inst.debug_messenger;

		// Get the surface of the window we opened with SDL
		SDL_Vulkan_CreateSurface(SDL_window, vkInstance, &vkSurface);

		// Use vkbootstrap to select a GPU.
		// We want a GPU that can write to the SDL surface and supports Vulkan 1.1
		vkb::PhysicalDeviceSelector selector{ vkb_inst };
		vkb::PhysicalDevice physicalDevice = selector
			.set_minimum_version(1, 1)
			.set_surface(vkSurface)
			.select()
			.value();

		// Create the final Vulkan device
		vkb::DeviceBuilder deviceBuilder{ physicalDevice };

		vkb::Device vkbDevice = deviceBuilder.build().value();

		// Get the VkDevice handle used in the rest of a Vulkan application
		vkDevice = vkbDevice.device;
		vkPhysicalDevice = physicalDevice.physical_device;

		// Build swap chain
		vkb::SwapchainBuilder swapchainBuilder{vkPhysicalDevice, vkDevice, vkSurface };

		vkb::Swapchain vkbSwapchain = swapchainBuilder
			.use_default_format_selection()
			// Use vsync present mode
			.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
			.set_desired_extent(windowExtent.width, windowExtent.height)
			.build()
			.value();

		// Store swapchain and its related images
		vkSwapchain = vkbSwapchain.swapchain;
		vkSwapchainImages = vkbSwapchain.get_images().value();
		vkSwapchainImageViews = vkbSwapchain.get_image_views().value();
		vkSwapchainImageFormat = vkbSwapchain.image_format;



		initialised = true;
	}

	void Renderer::shutdown(void)
	{
		if (initialised)
		{
			vkDestroySwapchainKHR(vkDevice, vkSwapchain, nullptr);

			// Destroy swapchain resources
			for (int i = 0; i < vkSwapchainImageViews.size(); i++) {

				vkDestroyImageView(vkDevice, vkSwapchainImageViews[i], nullptr);
			}

			vkDestroyDevice(vkDevice, nullptr);
			vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
			vkb::destroy_debug_utils_messenger(vkInstance, vkDebugMessenger);
			vkDestroyInstance(vkInstance, nullptr);
			SDL_DestroyWindow(SDL_window);
			SDL_window = nullptr;

			initialised = false;
		}
	}

	bool Renderer::update(void)
	{
		SDL_Event e;
		bool bQuit = false;
		while (SDL_PollEvent(&e) != 0)
			if (e.type == SDL_QUIT) bQuit = true;
		return !bQuit;
	}
}