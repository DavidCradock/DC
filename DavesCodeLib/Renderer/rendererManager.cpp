#include "rendererManager.h"
#include "../Common/error.h"
#include "rendererOpenGL.h"
#include "rendererVulkan.h"

namespace DC
{
	class RendererManager::impl
	{
	};

	RendererManager::RendererManager()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
		renderer = 0;
	}

	RendererManager::~RendererManager()
	{
		delete pimp;
		pimp = 0;
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