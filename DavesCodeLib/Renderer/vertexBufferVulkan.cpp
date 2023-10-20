#include "vertexBufferVulkan.h"
#include "../Common/error.h"

namespace DC
{
	class VertexBufferVulkan::impl
	{
	};

	VertexBufferVulkan::VertexBufferVulkan()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	VertexBufferVulkan::~VertexBufferVulkan()
	{
		delete pimp;
		pimp = 0;
	}
}
