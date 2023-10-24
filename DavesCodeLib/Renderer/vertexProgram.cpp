#include "vertexProgram.h"
#include "../Common/error.h"

namespace DC
{
	class VertexProgram::impl
	{
	};

	VertexProgram::VertexProgram()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	VertexProgram::~VertexProgram()
	{
		delete pimp;
		pimp = 0;
	}
}
