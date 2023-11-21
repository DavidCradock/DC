#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace DC
{
	// The VkPipeline is a huge object in Vulkan that encompasses the configuration of the entire GPU for the draw.
	// Building them can be very expensive, as it will fully convert the shader module into the GPU instructions, and will validate the setup for it.
	// Once a pipeline is built, it can be bound inside a command buffer, and then when you draw anything it will use the bound pipeline.
	// Vulkan pipelines are a massive object with many different configuration structs, some of them even running pointers and being arrays.
	// For this reason, we are going to create a class specifically for building pipelines, that will simplify the process.
	class PipelineBuilder
	{
	public:

		std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
		VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
		VkViewport _viewport;
		VkRect2D _scissor;
		VkPipelineRasterizationStateCreateInfo _rasterizer;
		VkPipelineColorBlendAttachmentState _colorBlendAttachment;
		VkPipelineMultisampleStateCreateInfo _multisampling;
		VkPipelineLayout _pipelineLayout;

		VkPipeline build_pipeline(VkDevice device, VkRenderPass pass);
	};


}