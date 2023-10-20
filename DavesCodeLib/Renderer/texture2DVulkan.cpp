#include "texture2DVulkan.h"
#include "../Common/error.h"

namespace DC
{
	class Texture2DVulkan::impl
	{
	};

	Texture2DVulkan::Texture2DVulkan()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	Texture2DVulkan::~Texture2DVulkan()
	{
		delete pimp;
		pimp = 0;
	}
}
