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
	String err(L"Error");
	std::wstring widestring = L"Hello";
	String s2(widestring);
	String s3(L"Hello");
	String s4(s3);

	String sfilename("image.ext");
	sfilename.addFilenameExtension(String(L"png"));
	return 0;
}