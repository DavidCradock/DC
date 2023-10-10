#pragma once
#include "rendererOpenGL.h"
#include "rendererVulkan.h"

namespace DC
{
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
	};
}