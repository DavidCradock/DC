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
	String stringA(L"stringA");
	String stringB(L"stringB");
	String stringC;
	stringC += stringA;
	stringC += L"_";
	stringC += stringB;
	stringC = stringA;
	stringA = L"A";
	stringB = L"A";
	stringA = L"Line1\nLine2\nLine3";
	std::vector<String> vLines = stringA.splitString();
	wchar_t wpi[] = L"\u03c0";
	int dfh = 4;

	return 0;
}