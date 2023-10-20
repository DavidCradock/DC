#pragma once
#include "vertexBufferBase.h"

namespace DC
{
	class VertexBufferOpenGL : public VertexBufferBase
	{
	public:
		VertexBufferOpenGL();
		~VertexBufferOpenGL();

	private:
		class impl;
		impl* pimp;
	};
}