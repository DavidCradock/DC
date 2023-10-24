#pragma once

namespace DC
{
	// A Vulkan API GPU vertex program
	class VertexProgram
	{
	public:
		VertexProgram();
		~VertexProgram();

	private:
		class impl;
		impl* pimp;
	};
}