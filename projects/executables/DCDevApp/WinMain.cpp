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
	String sfilename(L"filename");
	sfilename.addFilenameExtension(String(L"png"));
	return 0;
}