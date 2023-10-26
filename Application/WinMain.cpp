#include "mainApplication.h"
#include <Windows.h>

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	MainApplication* pMyApp = new MainApplication;
	ErrorIfMemoryNotAllocated(pMyApp);
	DC::Framework* pFramework = new DC::Framework;
	ErrorIfMemoryNotAllocated(pFramework);
	pFramework->go(pMyApp);
	return 0;
}