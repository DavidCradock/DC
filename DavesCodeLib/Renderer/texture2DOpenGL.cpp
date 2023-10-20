#include "texture2DOpenGL.h"
#include "../Common/error.h"

namespace DC
{
	class Texture2DOpenGL::impl
	{
	};

	Texture2DOpenGL::Texture2DOpenGL()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	Texture2DOpenGL::~Texture2DOpenGL()
	{
		delete pimp;
		pimp = 0;
	}
}
