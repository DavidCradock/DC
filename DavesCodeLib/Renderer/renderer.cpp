#include "renderer.h"
#include "../Common/error.h"
#include "vkError.h"
#include "../../third-party/vk-bootstrap-main/src/VkBootstrap.h"
#include "vk_initializers.h"

/*
Vulkan main objects and their use.

	VkInstance : The Vulkan context, used to access drivers.
	VkPhysicalDevice : A GPU. Used to query physical GPU details, like features, capabilities, memory size, etc.
	VkDevice : The �logical� GPU context that you actually execute things on.
	VkBuffer : A chunk of GPU visible memory.
	VkImage : A texture you can write to and read from.
	VkPipeline : Holds the state of the gpu needed to draw. For example: shaders, rasterization options, depth settings.
	VkRenderPass : Holds information about the images you are rendering into. All drawing commands have to be done inside a render pass.
	VkFrameBuffer : Holds the target images for a render pass.
	VkCommandBuffer : Encodes GPU commands. All execution that is performed on the GPU itself (not in the driver) has to be encoded in a VkCommandBuffer.
	VkQueue : Execution �port� for commands. GPUs will have a set of queues with different properties. Some allow only graphics commands, others only allow memory commands, etc. Command buffers are executed by submitting them into a queue, which will copy the rendering commands onto the GPU for execution.
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
VkImage resources, making sure that the images are in �readable� layout. You also create VkRenderPass objects for whatever your main rendering passes you have. For example, there may
be a VkRenderPass for the main rendering, and another for a shadow pass. On a real engine, much of this can be parallelized and done in background threads, especially since pipeline
creation can be quite expensive.


Render Loop

Now that everything is ready for rendering, you first ask the VkSwapchainKHR for an image to render to. Then you allocate a VkCommandBuffer from a VkCommandBufferPool or reuse an
already allocated command buffer that has finished execution, and �start� the command buffer, which allows you to write commands into it. Next, you begin rendering by starting a
VkRenderPass, which specifies that you are rendering to the image requested from swapchain, then create a loop where you bind a VkPipeline, bind some VkDescriptorSet resources
(for the shader parameters), bind the vertex buffers, and then execute a draw call. Once you are finished with the drawing for a pass, you end the VkRenderPass. If there is nothing
more to render, you end the VkCommandBuffer. Finally, you submit the command buffer into the queue for rendering. This will begin execution of the commands in the command buffer on
the gpu. If you want to display the result of the rendering, you �present� the image you have rendered to to the screen. Because the execution may not have finished yet, you use a
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

		initVulkan();
		initSwapchain();
		initCommands();
		initDefaultRenderpass();
		initFramebuffers();

		initialised = true;
	}

	void Renderer::initVulkan(void)
	{
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

		// Use vkbootstrap to get a Graphics queue
		_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
		_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
	}

	void Renderer::initSwapchain(void)
	{
		// Build swap chain
		vkb::SwapchainBuilder swapchainBuilder{ vkPhysicalDevice, vkDevice, vkSurface };

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
	}

	void Renderer::initCommands(void)
	{
		// Create a command pool for commands submitted to the graphics queue.
		// We also want the pool to allow for resetting of individual command buffers
		VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		vkError(vkCreateCommandPool(vkDevice, &commandPoolInfo, nullptr, &_commandPool));

		// Allocate the default command buffer that we will use for rendering
		VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_commandPool, 1);
		vkError(vkAllocateCommandBuffers(vkDevice, &cmdAllocInfo, &_mainCommandBuffer));
	}

	void Renderer::initDefaultRenderpass(void)
	{
		// The renderpass will use this colour attachment.
		VkAttachmentDescription colour_attachment = {};
		
		// The attachment will have the format needed by the swapchain
		colour_attachment.format = vkSwapchainImageFormat;

		// 1 sample, we won't be doing MSAA
		colour_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		
		// We Clear when this attachment is loaded
		colour_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

		// We keep the attachment stored when the renderpass ends
		colour_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		// We don't care about stencil
		colour_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colour_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		// We don't know or care about the starting layout of the attachment
		colour_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		// After the renderpass ends, the image has to be on a layout ready for display
		colour_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// Now that our main image target is defined, we need to add a subpass that will render into it.
		VkAttachmentReference colour_attachment_ref = {};

		// Attachment number will index into the pAttachments array in the parent renderpass itself
		colour_attachment_ref.attachment = 0;
		colour_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// We are going to create 1 subpass, which is the minimum you can do
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colour_attachment_ref;

		// Create render pass
		VkRenderPassCreateInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

		// Connect the color attachment to the info
		render_pass_info.attachmentCount = 1;
		render_pass_info.pAttachments = &colour_attachment;
		// Connect the subpass to the info
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;
		vkError(vkCreateRenderPass(vkDevice, &render_pass_info, nullptr, &_renderPass));
	}

	void Renderer::initFramebuffers(void)
	{
		// Create the framebuffers for the swapchain images.
		// This will connect the render-pass to the images for rendering
		VkFramebufferCreateInfo fb_info = {};
		fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fb_info.pNext = nullptr;

		fb_info.renderPass = _renderPass;
		fb_info.attachmentCount = 1;
		fb_info.width = windowExtent.width;
		fb_info.height = windowExtent.height;
		fb_info.layers = 1;

		// Grab how many images we have in the swapchain
		const size_t swapchain_imagecount = vkSwapchainImages.size();
		_framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

		// Create framebuffers for each of the swapchain image views
		for (size_t i = 0; i < swapchain_imagecount; i++) {

			fb_info.pAttachments = &vkSwapchainImageViews[i];
			vkError(vkCreateFramebuffer(vkDevice, &fb_info, nullptr, &_framebuffers[i]));
		}
	}

	void Renderer::shutdown(void)
	{
		if (initialised)
		{
			vkDestroySwapchainKHR(vkDevice, vkSwapchain, nullptr);

			// Destroy the main renderpass
			vkDestroyRenderPass(vkDevice, _renderPass, nullptr);

			// Destroy swapchain resources
			for (size_t i = 0; i < _framebuffers.size(); i++) {
				vkDestroyFramebuffer(vkDevice, _framebuffers[i], nullptr);

				vkDestroyImageView(vkDevice, vkSwapchainImageViews[i], nullptr);
			}

			vkDestroyCommandPool(vkDevice, _commandPool, nullptr);

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