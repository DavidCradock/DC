#include "vertexProgramOpenGL.h"
#include "../Common/error.h"

namespace DC
{
	class VertexProgramOpenGL::impl
	{
	};

	VertexProgramOpenGL::VertexProgramOpenGL()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	VertexProgramOpenGL::~VertexProgramOpenGL()
	{
		delete pimp;
		pimp = 0;
	}
}
