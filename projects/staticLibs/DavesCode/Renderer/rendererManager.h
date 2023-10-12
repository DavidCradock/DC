#pragma once
#include "rendererBase.h"
#include <memory>

namespace DC
{
	// Creates and returns a graphics renderer used for creating a window, hooking
	// up a graphics API to it and performing rendering operations in that window.
	class RendererManager
	{
	public:
		RendererManager();
		~RendererManager();

		Renderer* initOpenGL(void);
		Renderer* initVulkan(void);
		Renderer* get(void);
	private:
		Renderer* renderer;

		class impl;
		std::unique_ptr<impl> pimpl;
	};
}