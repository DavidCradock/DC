#include "vertexBuffer.h"
#include "../Common/error.h"

namespace DC
{
	class VertexBuffer::impl
	{
	};

	VertexBuffer::VertexBuffer()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	VertexBuffer::~VertexBuffer()
	{
		delete pimp;
		pimp = 0;
	}
}
