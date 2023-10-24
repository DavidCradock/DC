#include "renderer.h"
#include "../Common/error.h"
#include "../../third-party/SDL2-2.28.4/include/SDL.h"

namespace DC
{
	class Renderer::impl
	{
	public:
	};

	Renderer::Renderer()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	Renderer::~Renderer()
	{
		delete pimp;
		pimp = 0;
	}

	void Renderer::init(void)
	{

	}

	void Renderer::shutdown(void)
	{

	}

	void Renderer::blendDisable(void)
	{

	}

	void Renderer::blendEnable(void)
	{

	}

	void Renderer::blendFunction_SrcAlpha_One(void)
	{

	}

	void Renderer::blendFunction_SrcAlpha_OneMinusSrcAlpha(void)
	{

	}

	void Renderer::cullFaceAntiClockwise(void)
	{

	}

	void Renderer::cullFaceClockwise(void)
	{

	}

	void Renderer::cullFaceDisable(void)
	{

	}

	void Renderer::cullFaceEnable(void)
	{

	}

	void Renderer::depthTestDisable(void)
	{

	}

	void Renderer::depthTestEnable(void)
	{

	}

	void Renderer::pixelsRead(int positionX, int positionY, int width, int height, void* pixels)
	{

	}

	void Renderer::pixelsWrite(int positionX, int positionY, int width, int height, void* pixels)
	{

	}

	void Renderer::scissorTest(int positionX, int positionY, int width, int height)
	{

	}

	void Renderer::scissorTestEnable(void)
	{

	}

	void Renderer::scissorTestDisable(void)
	{

	}
}