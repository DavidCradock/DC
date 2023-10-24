#pragma once

namespace DC
{
	// A Vulkan API 2D texture
	class Texture2D
	{
	public:
		Texture2D();
		~Texture2D();

	private:
		class impl;
		impl* pimp;
	};
}