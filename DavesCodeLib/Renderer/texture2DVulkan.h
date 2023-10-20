#pragma once
#include "texture2DBase.h"

namespace DC
{
	class Texture2DVulkan : public Texture2DBase
	{
	public:
		Texture2DVulkan();
		~Texture2DVulkan();

	private:
		class impl;
		impl* pimp;
	};
}