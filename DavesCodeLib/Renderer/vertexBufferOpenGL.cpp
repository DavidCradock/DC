#include "vertexBufferOpenGL.h"
#include "../Common/error.h"

namespace DC
{
	class VertexBufferOpenGL::impl
	{
	};

	VertexBufferOpenGL::VertexBufferOpenGL()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	VertexBufferOpenGL::~VertexBufferOpenGL()
	{
		delete pimp;
		pimp = 0;
	}
}
