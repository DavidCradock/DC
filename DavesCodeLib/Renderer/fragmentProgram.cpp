#include "fragmentProgramVulkan.h"
#include "../../Common/error.h"

namespace DC
{
	class FragmentProgramVulkan::impl
	{
	};

	FragmentProgramVulkan::FragmentProgramVulkan()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	FragmentProgramVulkan::~FragmentProgramVulkan()
	{
		delete pimp;
		pimp = 0;
	}
}
