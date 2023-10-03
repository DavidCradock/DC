#include <Windows.h>
#include "DCIncludes.h"

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
		name.appendInt((int)i);
		mapTest[name] = (int)i;
	}
	profiler.end(sectionMapInit);

	
	for (size_t i = 0; i < 10000; i++)
	{
		String name;
		name.append(L"SomeLongText");
		name.appendInt((int)i);
		profiler.begin(sectionMapFind);
		auto it = mapTest.find(name);
		profiler.end(sectionMapFind);
	}

	String test;
	test += L"\u2211";


	
	profiler.end(sectionMain);

	std::vector<ProfilerResults> results = profiler.getResults(false);

	return 0;
}