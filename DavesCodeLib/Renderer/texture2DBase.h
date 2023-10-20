#pragma once

namespace DC
{
	// Base class which declares which methods need to be in a derived texture2D class.
	// No implementation here
	class Texture2DBase
	{
	public:
		// Binds the texture to the specified texture unit
		virtual void bind(unsigned int textureUnit) = 0;

	};
}