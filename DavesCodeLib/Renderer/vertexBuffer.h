#pragma once
#include "vertexBuffer.h"

namespace DC
{
	// A Vulkan API GPU vertex buffer
	class VertexBuffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();

	private:
		class impl;
		impl* pimp;
	};
}