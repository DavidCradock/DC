#pragma once
#include "texture2DBase.h"

namespace DC
{
	class Texture2DOpenGL : public Texture2DBase
	{
	public:
		Texture2DOpenGL();
		~Texture2DOpenGL();

	private:
		class impl;
		impl* pimp;
	};
}