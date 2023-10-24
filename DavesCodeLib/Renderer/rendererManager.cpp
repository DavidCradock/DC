#include "rendererManager.h"
#include "../../Common/error.h"
#include "renderer.h"

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

	Renderer* RendererManager::init(void)
	{
		ErrorIfTrue(renderer, L"RendererManager::init() failed. Renderer already initialised.");
		renderer = new Renderer;
		ErrorIfMemoryNotAllocated(renderer);
		return renderer;
	}
	Renderer* RendererManager::get(void)
	{
		ErrorIfFalse(renderer, L"RendererManager::get() failed. Renderer is not initialised.");
		return renderer;
	}
}