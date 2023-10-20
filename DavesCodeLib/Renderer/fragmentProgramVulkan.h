#pragma once
#include "fragmentProgramBase.h"

namespace DC
{
	class FragmentProgramVulkan : public FragmentProgramBase
	{
	public:
		FragmentProgramVulkan();
		~FragmentProgramVulkan();

	private:
		class impl;
		impl* pimp;
	};
}