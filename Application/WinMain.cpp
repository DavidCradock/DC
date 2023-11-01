#include "mainApplication.h"
#include <Windows.h>

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	MainApplication* pMyApp = new MainApplication;
	ErrorIfMemoryNotAllocated(pMyApp);

	DC::Engine* pEngine = new DC::Engine;
	ErrorIfMemoryNotAllocated(pEngine);
	
	pEngine->go(pMyApp);
	return 0;
}