#pragma once
#include "fragmentProgram.h"

namespace DC
{
	class FragmentProgramVulkan : public FragmentProgram
	{
	public:
		FragmentProgramVulkan();
		~FragmentProgramVulkan();

	private:
		class impl;
		impl* pimp;
	};
}