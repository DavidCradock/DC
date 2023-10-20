#include "fragmentProgramOpenGL.h"
#include "../Common/error.h"

namespace DC
{
	class FragmentProgramOpenGL::impl
	{
	};

	FragmentProgramOpenGL::FragmentProgramOpenGL()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	FragmentProgramOpenGL::~FragmentProgramOpenGL()
	{
		delete pimp;
		pimp = 0;
	}
}
