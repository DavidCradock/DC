#include "renderer.h"
#include "rendererPimp.h"
#include "../Common/error.h"
#include "vkError.h"
#include <vulkan/vulkan.h>
#include <SDL.h>
#include "../Common/logging.h"

namespace DC
{
	
	Renderer::Renderer()
	{
		pimp = new Renderer::Pimpl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	Renderer::~Renderer()
	{
		delete pimp;
		pimp = 0;
	}

	void Renderer::init(Settings& settings)
	{
		log.addEntryINFO(L"Renderer::init() called.");
		pimp->init(settings);
	}

	void Renderer::shutdown(void)
	{
		log.addEntryINFO(L"Renderer::shutdown() called.");
		pimp->shutdown();
	}

	bool Renderer::update(void)
	{
		return pimp->update();
	}
}