#pragma once
#include "fragmentProgramManager.h"
#include "texture2DManager.h"
#include "vertexBufferManager.h"
#include "vertexProgramManager.h"
#include "../Engine/settings.h"
#include "../../third-party/SDL2-2.28.4/include/SDL.h"
#include "../../third-party/SDL2-2.28.4/include/SDL_vulkan.h"
#include <vulkan/vulkan.h>

namespace DC
{
	// Vulkan renderer.
	// Creates and returns a graphics renderer used for creating a window, hooking
	// up the Vulkan graphics API to it and performing rendering operations in that window.
	// The application framework has an object of this class. See Framework/framework.h
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		// Creates a window and hooks up the Vulkan graphics API to it, ready for rendering
		void init(const Settings &settings);

		// Closes the window and cleans everything up
		void shutdown(void);

		// Called each program loop updating the window etc.
		// Returns false if the window has been asked to close or alt+F4 has been pressed
		bool update(void);

		// The fragment program manager.
		FragmentProgramManager managerFragmentProgram;

		// The texture2D manager.
		Texture2DManager managerTexture2D;

		// The vertex buffer manager.
		VertexBufferManager managerVertexBuffer;

		// The vertex program manager.
		VertexProgramManager managerVertexProgram;
	private:
		// Called from init() to initialise Vulkan
		void initVulkan(void);

		// Called from init() to initialise the Vulkan swap chain
		void initSwapchain(void);

		// Called from init() to initialise the Vulkan command pool and buffer
		void initCommands(void);

		// Called from init() to initialise the Vulkan default render pass
		void initDefaultRenderpass(void);

		// Called from init() to initialise the Vulkan frame buffers
		void initFramebuffers(void);

		// Called from init() to initialise the Vulkan synchronization structures
		void initSyncStructures(void);

		// ?
		void draw(void);

		// Loads a shader module from a spir-v file. Returns false if it errors
		bool load_shader_module(const char* filePath, VkShaderModule* outShaderModule);

		// ?
		void initPipelines(void);

		// The SDL window handle
		SDL_Window* SDL_window{ nullptr };

		// Whether init() has been called
		bool initialised{ false };

		// Width and height of the renderer's window's dimensions
		VkExtent2D vkWindowExtent{ 1920, 1080 };

		// The Vulkan context, used to access drivers.
		VkInstance vkInstance;

		// A GPU. Used to query physical GPU details, like features, capabilities, memory size, etc.
		VkPhysicalDevice vkPhysicalDevice;

		// The “logical” GPU context that you actually execute things on.
		VkDevice vkDevice;

		// Vulkan debug output handle
		VkDebugUtilsMessengerEXT vkDebugMessenger;

		// Vulkan window surface
		VkSurfaceKHR vkSurface;

		// Vulkan swap chain
		VkSwapchainKHR vkSwapchain;

		// Image format expected by the windowing system
		VkFormat vkSwapchainImageFormat;

		// Array of images from the swapchain
		std::vector<VkImage> vkSwapchainImages;

		// Array of image-views from the swapchain
		std::vector<VkImageView> vkSwapchainImageViews;

		// Queue we will submit to
		VkQueue vkGraphicsQueue;
		
		// Family of that queue
		uint32_t vkGraphicsQueueFamily;

		// The command pool for our commands
		VkCommandPool vkCommandPool;

		// The buffer we will record into
		VkCommandBuffer vkMainCommandBuffer;

		// Vulkan render pass
		VkRenderPass vkRenderPass;

		// Vulkan frame buffers
		std::vector<VkFramebuffer> vkFramebuffers;

		// Vulkan synchronization
		VkSemaphore vkPresentSemaphore, vkRenderSemaphore;
		
		// Vulkan synchronization
		VkFence vkRenderFence;

		// ?
		int _frameNumber{ 0 };

		VkPipelineLayout _trianglePipelineLayout;

		VkPipeline _trianglePipeline;
	};
}