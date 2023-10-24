#include "texture2D.h"
#include "../Common/error.h"

namespace DC
{
	class Texture2D::impl
	{
	};

	Texture2D::Texture2D()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	Texture2D::~Texture2D()
	{
		delete pimp;
		pimp = 0;
	}
}
