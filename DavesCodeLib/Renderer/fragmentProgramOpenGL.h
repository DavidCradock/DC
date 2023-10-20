#pragma once
#include "fragmentProgramBase.h"

namespace DC
{
	class FragmentProgramOpenGL : public FragmentProgramBase
	{
	public:
		FragmentProgramOpenGL();
		~FragmentProgramOpenGL();

	private:
		class impl;
		impl* pimp;
	};
}