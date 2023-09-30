#include <Windows.h>
#include "../../staticLibs/DCCommon/DCCommon.h"
#ifdef _DEBUG
#pragma comment(lib, "../../../x64/Debug/DCCommon.lib")
#else
#pragma comment(lib, "../../../x64/Release/DCCommon.lib")
#endif

using namespace DC;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Profiler profiler;
	String sectionMain(L"main");
	String sectionMapInit(L"mapInit");
	String sectionMapFind(L"mapFind");
	profiler.begin(sectionMain);
	
	profiler.begin(sectionMapInit);

	std::map<String, int> mapTest;
	for (size_t i = 0; i < 10000; i++)
	{
		String name;
		name.append(L"SomeLongText");
		name.appendInt(i);
		mapTest[name] = i;
	}
	profiler.end(sectionMapInit);

	
	for (size_t i = 0; i < 10000; i++)
	{
		String name;
		name.append(L"SomeLongText");
		name.appendInt(i);
		profiler.begin(sectionMapFind);
		auto it = mapTest.find(name);
		profiler.end(sectionMapFind);
	}

	
	
	profiler.end(sectionMain);

	std::vector<ProfilerResults> results = profiler.getResults(false);

	int bb;
	return 0;
}