#pragma once
#include "vertexBuffer.h"

namespace DC
{
	class VertexBuffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();

	private:
		class impl;
		impl* pimp;
	};
}