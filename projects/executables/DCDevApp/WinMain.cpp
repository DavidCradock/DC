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
	try
	{
		String stringA("stringA");
		String stringB("stringB");
		String stringC;
		stringC += stringA;
		stringC += "_";
		stringC += stringB;
		stringC = stringA;
		stringA = "A";
		stringB = "A";
		stringA = "Line1\nLine2\nLine3";
		std::vector<String> vLines = stringA.splitString();
		ErrorIfFalse(0, "false!");
		int dfh = 4;
	}
	catch (Exception &ex)
	{
		String strTemp = ex.description;
		int i = 3;
	}
	return 0;
}