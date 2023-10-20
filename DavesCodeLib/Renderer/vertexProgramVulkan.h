#pragma once
#include "vertexProgramBase.h"

namespace DC
{
	class VertexProgramVulkan : public VertexProgramBase
	{
	public:
		VertexProgramVulkan();
		~VertexProgramVulkan();

	private:
		class impl;
		impl* pimp;
	};
}