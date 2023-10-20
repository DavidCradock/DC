#include "vertexProgramVulkan.h"
#include "../Common/error.h"

namespace DC
{
	class VertexProgramVulkan::impl
	{
	};

	VertexProgramVulkan::VertexProgramVulkan()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	VertexProgramVulkan::~VertexProgramVulkan()
	{
		delete pimp;
		pimp = 0;
	}
}
