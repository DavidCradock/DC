#include <Windows.h>
#include "../DavesCodeLib/DavesCode.h"
using namespace DC;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	RendererManager* pRM = new RendererManager;
	ErrorIfMemoryNotAllocated(pRM);
	Renderer* pRenderer = pRM->initVulkan();

	return 0;
}