#include "fragmentProgram.h"
#include "../Common/error.h"

namespace DC
{
	class FragmentProgram::impl
	{
	};

	FragmentProgram::FragmentProgram()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	FragmentProgram::~FragmentProgram()
	{
		delete pimp;
		pimp = 0;
	}
}
