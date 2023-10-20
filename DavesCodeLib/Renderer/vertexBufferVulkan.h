#pragma once
#include "vertexBufferBase.h"

namespace DC
{
	class VertexBufferVulkan : public VertexBufferBase
	{
	public:
		VertexBufferVulkan();
		~VertexBufferVulkan();

	private:
		class impl;
		impl* pimp;
	};
}