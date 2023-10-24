#pragma once
#include "renderer.h"

namespace DC
{
	// Creates and returns a graphics renderer used for creating a window, hooking
	// up a graphics API to it and performing rendering operations in that window.
	// See rendererBase.h for more information with the Renderer class.
	class RendererManager
	{
	public:
		RendererManager();
		~RendererManager();

		Renderer* init(void);
		Renderer* get(void);
	private:
		Renderer* renderer;

		class impl;
		impl* pimp;
	};
}