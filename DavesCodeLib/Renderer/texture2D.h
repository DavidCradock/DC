#pragma once

namespace DC
{
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