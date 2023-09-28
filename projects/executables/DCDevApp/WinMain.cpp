#include <Windows.h>
#include "../../staticLibs/DCCommon/DCCommon.h"
#ifdef _DEBUG
#pragma comment(lib, "../../../x64/Debug/DCCommon.lib")
#else
#pragma comment(lib, "../../../x64/Release/DCCommon.lib")
#endif

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	DCString string;
	DCString::iterator it = string.begin();
	while (it != string.end())
	{
		it++;
	}

	return 0;
}