#include "rendererManager.h"
#include "../DCCommon/error.h"

namespace DC
{
	RendererManager::RendererManager()
	{
		renderer = 0;
	}

	RendererManager::~RendererManager()
	{

	}

	Renderer* RendererManager::initOpenGL(void)
	{
		ErrorIfTrue(renderer, L"RendererManager::initOpenGL() failed. Renderer already initialised.");
		renderer = new RendererOpenGL;
		ErrorIfMemoryNotAllocated(renderer);
		return renderer;
	}

	Renderer* RendererManager::initVulkan(void)
	{
		ErrorIfTrue(renderer, L"RendererManager::initVulkan() failed. Renderer already initialised.");
		renderer = new RendererVulkan;
		ErrorIfMemoryNotAllocated(renderer);
		return renderer;
	}
	Renderer* RendererManager::get(void)
	{
		ErrorIfFalse(renderer, L"RendererManager::get() failed. Renderer is not initialised.");
		return renderer;
	}
}