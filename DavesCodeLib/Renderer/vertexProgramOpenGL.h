#pragma once
#include "vertexProgramBase.h"

namespace DC
{
	class VertexProgramOpenGL : public VertexProgramBase
	{
	public:
		VertexProgramOpenGL();
		~VertexProgramOpenGL();

	private:
		class impl;
		impl* pimp;
	};
}