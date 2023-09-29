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
	String str(L"filename");
	str.addFilenameExtension(String(L"png"));
	str.clear();

	str.append(L"_Hello");
	str.clear();

	str.appendDouble(3.14, 2);
	str.clear();

	str.appendFloat(3.14f, 2);
	str.clear();

	str.appendInt(3);
	str.clear();

	str.appendUnsignedInt(3);
	str.clear();

	str.lowercase();
	str += L"Hello";
	str.lowercase();
	str.clear();

	str.append(L"3.14");
	bool bIsNum = str.representsNumber();
	str.clear();

	str.append(L"Hello");
	str += L"SPLITCHARS";
	str += L"world";
	std::vector<String> vstr = str.splitString(L"SPLITCHARS");
	str.clear();

	str += L"Hello again!";
	std::string strNarrow = str.wideCharToMultiByte();
	str.clear();

	strNarrow.clear();
	strNarrow += "Hello! I'm a narrow string.";
	str.multiByteToWideChar(strNarrow);

	return 0;
}