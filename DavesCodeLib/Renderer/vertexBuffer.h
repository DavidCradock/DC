#pragma once
#include "../Common/string.h"

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