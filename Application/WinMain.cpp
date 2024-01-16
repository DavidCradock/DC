#include "mainApplication.h"
#include <Windows.h>

void someTest(const DC::String& string, DC::Logging::EntryStatus status)
{
	// Do stuff with the parsed data
}

#ifdef NDEBUG
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#else
int main()
{
#endif

	MainApplication* pMyApp = new MainApplication;
	ErrorIfMemoryNotAllocated(pMyApp);

	DC::Engine* pEngine = new DC::Engine;
	ErrorIfMemoryNotAllocated(pEngine);
	
	DC::log.setFunctionToCallOnAddEntry(someTest);
	pEngine->go(pMyApp);
	return 0;
}