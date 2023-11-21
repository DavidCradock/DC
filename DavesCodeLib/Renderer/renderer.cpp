#include "renderer.h"
#include "../Common/error.h"
#include "vkError.h"
#include "../../third-party/vk-bootstrap-main/src/VkBootstrap.h"
#include "vk_initializers.h"
#include "pipelineBuilder.h"

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
		vkWindowExtent.width = settings.getWindowWidthWhenWindowed();
		vkWindowExtent.height = settings.getWindowHeightWhenWindowed();

		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
		SDL_window = SDL_CreateWindow(
			"Dave's Code: Vulkan Engine",
			SDL_WINDOWPOS_UNDEFINED, // Window position x (don't care)
			SDL_WINDOWPOS_UNDEFINED, // Window position y (don't care)
			vkWindowExtent.width, // Window width in pixels
			vkWindowExtent.height, // Window height in pixels
			window_flags
		);

		initVulkan();
		initSwapchain();
		initCommands();
		initDefaultRenderpass();
		initFramebuffers();
		initSyncStructures();
		initPipelines();

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
		vkGraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
		vkGraphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
	}

	void Renderer::initSwapchain(void)
	{
		// Build swap chain
		vkb::SwapchainBuilder swapchainBuilder{ vkPhysicalDevice, vkDevice, vkSurface };

		vkb::Swapchain vkbSwapchain = swapchainBuilder
			.use_default_format_selection()
			// Use vsync present mode
			.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
			.set_desired_extent(vkWindowExtent.width, vkWindowExtent.height)
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
		VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(vkGraphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		vkError(vkCreateCommandPool(vkDevice, &commandPoolInfo, nullptr, &vkCommandPool));

		// Allocate the default command buffer that we will use for rendering
		VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(vkCommandPool, 1);
		vkError(vkAllocateCommandBuffers(vkDevice, &cmdAllocInfo, &vkMainCommandBuffer));
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
		vkError(vkCreateRenderPass(vkDevice, &render_pass_info, nullptr, &vkRenderPass));
	}

	void Renderer::initFramebuffers(void)
	{
		// Create the framebuffers for the swapchain images.
		// This will connect the render-pass to the images for rendering
		VkFramebufferCreateInfo fb_info = {};
		fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fb_info.pNext = nullptr;

		fb_info.renderPass = vkRenderPass;
		fb_info.attachmentCount = 1;
		fb_info.width = vkWindowExtent.width;
		fb_info.height = vkWindowExtent.height;
		fb_info.layers = 1;

		// Grab how many images we have in the swapchain
		const size_t swapchain_imagecount = vkSwapchainImages.size();
		vkFramebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

		// Create framebuffers for each of the swapchain image views
		for (size_t i = 0; i < swapchain_imagecount; i++) {

			fb_info.pAttachments = &vkSwapchainImageViews[i];
			vkError(vkCreateFramebuffer(vkDevice, &fb_info, nullptr, &vkFramebuffers[i]));
		}
	}

	void Renderer::initSyncStructures(void)
	{
		// Create synchronization structures

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;

		// We want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		vkError(vkCreateFence(vkDevice, &fenceCreateInfo, nullptr, &vkRenderFence));

		// For the semaphores we don't need any flags
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		vkError(vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, nullptr, &vkPresentSemaphore));
		vkError(vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, nullptr, &vkRenderSemaphore));
	}

	void Renderer::shutdown(void)
	{
		if (initialised)
		{
			vkDestroySwapchainKHR(vkDevice, vkSwapchain, nullptr);

			// Destroy the main renderpass
			vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);

			// Destroy swapchain resources
			for (size_t i = 0; i < vkFramebuffers.size(); i++) {
				vkDestroyFramebuffer(vkDevice, vkFramebuffers[i], nullptr);

				vkDestroyImageView(vkDevice, vkSwapchainImageViews[i], nullptr);
			}

			vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);

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

		draw();

		return !bQuit;
	}

	void Renderer::draw(void)
	{
		// Wait until the GPU has finished rendering the last frame. Timeout of 1 second
		// The timeout of the WaitFences call is of 1 second. It’s using nanoseconds for the wait time.
		// If you call the function with 0 as the timeout, you can use it to know if the GPU is still executing the command or not.
		vkError(vkWaitForFences(vkDevice, 1, &vkRenderFence, true, 1000000000));
		vkError(vkResetFences(vkDevice, 1, &vkRenderFence));

		// Request image from the swapchain, one second timeout
		uint32_t swapchainImageIndex;
		vkError(vkAcquireNextImageKHR(vkDevice, vkSwapchain, 1000000000, vkPresentSemaphore, nullptr, &swapchainImageIndex));

		// Now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
		vkError(vkResetCommandBuffer(vkMainCommandBuffer, 0));

		// Naming it cmd for shorter writing
		VkCommandBuffer cmd = vkMainCommandBuffer;

		// Begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.pNext = nullptr;

		cmdBeginInfo.pInheritanceInfo = nullptr;
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkError(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		// Make a clear-color from frame number. This will flash with a 120*pi frame period.
		VkClearValue clearValue;
		float flash = abs(sin(_frameNumber / 60.f));
		clearValue.color = { { 0.0f, 0.0f, flash, 1.0f } };

		// Start the main renderpass.
		// We will use the clear color from above, and the framebuffer of the index the swapchain gave us
		VkRenderPassBeginInfo rpInfo = {};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rpInfo.pNext = nullptr;

		rpInfo.renderPass = vkRenderPass;
		rpInfo.renderArea.offset.x = 0;
		rpInfo.renderArea.offset.y = 0;
		rpInfo.renderArea.extent = vkWindowExtent;
		rpInfo.framebuffer = vkFramebuffers[swapchainImageIndex];

		// Connect clear values
		rpInfo.clearValueCount = 1;
		rpInfo.pClearValues = &clearValue;

		vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

		//once we start adding rendering commands, they will go here
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _trianglePipeline);
		vkCmdDraw(cmd, 3, 1, 0, 0);

		// Finalize the render pass
		vkCmdEndRenderPass(cmd);
		// Finalize the command buffer (we can no longer add commands, but it can now be executed)
		vkError(vkEndCommandBuffer(cmd));


		// Prepare the submission to the queue.
		// We want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
		// We will signal the _renderSemaphore, to signal that rendering has finished

		VkSubmitInfo submit = {};
		submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit.pNext = nullptr;

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		submit.pWaitDstStageMask = &waitStage;

		submit.waitSemaphoreCount = 1;
		submit.pWaitSemaphores = &vkPresentSemaphore;

		submit.signalSemaphoreCount = 1;
		submit.pSignalSemaphores = &vkRenderSemaphore;

		submit.commandBufferCount = 1;
		submit.pCommandBuffers = &cmd;

		// Submit command buffer to the queue and execute it.
		// _renderFence will now block until the graphic commands finish execution
		vkError(vkQueueSubmit(vkGraphicsQueue, 1, &submit, vkRenderFence));

		// This will put the image we just rendered into the visible window.
		// We want to wait on the _renderSemaphore for that,
		// as it's necessary that drawing commands have finished before the image is displayed to the user
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;

		presentInfo.pSwapchains = &vkSwapchain;
		presentInfo.swapchainCount = 1;

		presentInfo.pWaitSemaphores = &vkRenderSemaphore;
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.pImageIndices = &swapchainImageIndex;

		vkError(vkQueuePresentKHR(vkGraphicsQueue, &presentInfo));

		// Increase the number of frames drawn
		_frameNumber++;
	}

	bool Renderer::load_shader_module(const char* filePath, VkShaderModule* outShaderModule)
	{
		// Open the file. With cursor at the end
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			return false;
		}


		// Find what the size of the file is by looking up the location of the cursor
		// because the cursor is at the end, it gives the size directly in bytes
		size_t fileSize = (size_t)file.tellg();

		// Spirv expects the buffer to be on uint32, so make sure to reserve an int vector big enough for the entire file
		std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

		// Put file cursor at beginning
		file.seekg(0);

		// Load the entire file into the buffer
		file.read((char*)buffer.data(), fileSize);

		// Now that the file is loaded into the buffer, we can close it
		file.close();

		// Create a new shader module, using the buffer we loaded
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pNext = nullptr;

		// codeSize has to be in bytes, so multiply the ints in the buffer by size of int to know the real size of the buffer
		createInfo.codeSize = buffer.size() * sizeof(uint32_t);
		createInfo.pCode = buffer.data();

		// Check that the creation goes well.
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			return false;
		}
		*outShaderModule = shaderModule;
		return true;
	}

	void Renderer::initPipelines(void)
	{
		VkShaderModule triangleFragShader;
		ErrorIfFalse(load_shader_module("data/shaders/default.frag.spv", &triangleFragShader), L"Error when building the triangle fragment shader module.");

		VkShaderModule triangleVertexShader;
		ErrorIfFalse(load_shader_module("data/shaders/default.vert.spv", &triangleVertexShader), L"Error when building the triangle vertex shader module.");

		// Build the pipeline layout that controls the inputs/outputs of the shader
		// We are not using descriptor sets or other systems yet, so no need to use anything other than empty default
		VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();

		vkError(vkCreatePipelineLayout(vkDevice, &pipeline_layout_info, nullptr, &_trianglePipelineLayout));

		//build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage
		PipelineBuilder pipelineBuilder;

		pipelineBuilder._shaderStages.push_back(
			vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, triangleVertexShader));

		pipelineBuilder._shaderStages.push_back(
			vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));


		//vertex input controls how to read vertices from vertex buffers. We aren't using it yet
		pipelineBuilder._vertexInputInfo = vkinit::vertex_input_state_create_info();

		//input assembly is the configuration for drawing triangle lists, strips, or individual points.
		//we are just going to draw triangle list
		pipelineBuilder._inputAssembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		//build viewport and scissor from the swapchain extents
		pipelineBuilder._viewport.x = 0.0f;
		pipelineBuilder._viewport.y = 0.0f;
		pipelineBuilder._viewport.width = (float)vkWindowExtent.width;
		pipelineBuilder._viewport.height = (float)vkWindowExtent.height;
		pipelineBuilder._viewport.minDepth = 0.0f;
		pipelineBuilder._viewport.maxDepth = 1.0f;

		pipelineBuilder._scissor.offset = { 0, 0 };
		pipelineBuilder._scissor.extent = vkWindowExtent;

		//configure the rasterizer to draw filled triangles
		pipelineBuilder._rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);

		//we don't use multisampling, so just run the default one
		pipelineBuilder._multisampling = vkinit::multisampling_state_create_info();

		//a single blend attachment with no blending and writing to RGBA
		pipelineBuilder._colorBlendAttachment = vkinit::color_blend_attachment_state();

		//use the triangle layout we created
		pipelineBuilder._pipelineLayout = _trianglePipelineLayout;

		//finally build the pipeline
		_trianglePipeline = pipelineBuilder.build_pipeline(vkDevice, vkRenderPass);
	}

}