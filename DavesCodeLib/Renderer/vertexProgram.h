#pragma once

namespace DC
{
	class VertexProgram
	{
	public:
		VertexProgram();
		~VertexProgram();

	private:
		class impl;
		impl* pimp;
	};
}