#include "string.h"
#include <format>
#include <Windows.h>
#include <algorithm>
#include "error.h"

namespace DC
{

	String::String(void)
	{
		
	}

	String::String(const String& str)
	{
		wideString = str.wideString;
	}

	String::String(const char* str)
	{
		std::string nstr = str;
		multiByteToWideChar(nstr);
	}

	String::String(const wchar_t* str)
	{
		wideString = str;
	}

	String::String(const std::wstring& str)
	{
		wideString = str;
	}

	void String::operator+=(const String& str)
	{
		wideString += str.wideString;
	}

	bool String::operator==(const String& str) const
	{
		return wideString == str.wideString;
	}

	String String::operator +(const String& str) const
	{
		String result;
		result.append(wideString);
		result.append(str);
		return result;
	}

	void String::addFilenameExtension(const String& filenameExtension)
	{
		ErrorIfTrue(0 == filenameExtension.size(), String("String::addFilenameExtension() failed. Given extension name of zero length."));

		// Append "." to extension if needed
		String strExt = filenameExtension;
		if (strExt.c_str()[0] != '.')
		{
			std::wstring::iterator itBegin = strExt.wideString.begin();
			strExt.wideString.insert(itBegin, '.');
		}

		// Find last position of "." and remove everything after it
		auto const pos = wideString.find_last_of('.');
		if (pos != std::wstring::npos)
		{
			wideString.erase(pos, wideString.length() - pos);
		}

		wideString.append(strExt.wideString);
	}

	void String::append(const String& other)
	{
		wideString.append(other.wideString);
	}

	void String::append(const char* str)
	{
		std::string nstr = str;
		multiByteToWideChar(nstr);
	}

	void String::append(const wchar_t* str)
	{
		wideString.append(str);
	}

	void String::append(const std::wstring& str)
	{
		wideString = str;
	}

	void String::appendDouble(double dValue, unsigned int uiNumDecimalPoints)
	{
		switch (uiNumDecimalPoints)
		{
		case 0:
			wideString += std::format(L"{:.0f}", dValue);
			break;
		case 1:
			wideString += std::format(L"{:.1f}", dValue);
			break;
		case 2:
			wideString += std::format(L"{:.2f}", dValue);
			break;
		case 3:
			wideString += std::format(L"{:.3f}", dValue);
			break;
		case 4:
			wideString += std::format(L"{:.4f}", dValue);
			break;
		case 5:
			wideString += std::format(L"{:.5f}", dValue);
			break;
		case 6:
			wideString += std::format(L"{:.6f}", dValue);
			break;
		case 7:
			wideString += std::format(L"{:.7f}", dValue);
			break;
		case 8:
			wideString += std::format(L"{:.8f}", dValue);
			break;
		case 9:
			wideString += std::format(L"{:.9f}", dValue);
			break;
		default:
			wideString += std::format(L"{:.2f}", dValue);
		}
	}

	void String::appendFloat(float fValue, unsigned int uiNumDecimalPoints)
	{
		switch (uiNumDecimalPoints)
		{
		case 0:
			wideString += std::format(L"{:.0f}", fValue);
			break;
		case 1:
			wideString += std::format(L"{:.1f}", fValue);
			break;
		case 2:
			wideString += std::format(L"{:.2f}", fValue);
			break;
		case 3:
			wideString += std::format(L"{:.3f}", fValue);
			break;
		case 4:
			wideString += std::format(L"{:.4f}", fValue);
			break;
		case 5:
			wideString += std::format(L"{:.5f}", fValue);
			break;
		case 6:
			wideString += std::format(L"{:.6f}", fValue);
			break;
		case 7:
			wideString += std::format(L"{:.7f}", fValue);
			break;
		case 8:
			wideString += std::format(L"{:.8f}", fValue);
			break;
		case 9:
			wideString += std::format(L"{:.9f}", fValue);
			break;
		default:
			wideString += std::format(L"{:.2f}", fValue);
		}
	}

	void String::appendInt(int iInt)
	{
		wideString += std::to_wstring(iInt);
	}

	void String::appendUnsignedInt(unsigned int uiInt)
	{
		wideString += std::to_wstring(uiInt);
	}

	const wchar_t* String::c_str(void) const
	{
		return wideString.c_str();
	}

	void String::clear(void)
	{
		wideString.clear();
	}

	void String::lowercase(void)
	{
		std::string str = wideCharToMultiByte();
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
		multiByteToWideChar(str);
	}

	bool String::representsNumber(void) const
	{
		return (std::all_of(wideString.begin(), wideString.end(), ::isdigit));
	}

	size_t String::size(void) const
	{
		return wideString.size();
	}

	std::vector<String> String::splitString(const String& strSplitChars) const
	{
		std::vector<String> out;

		// If no strSplitChars found, simply add the entire string and return the result
		size_t pos = wideString.find(strSplitChars.wideString, 0);
		if (std::string::npos == pos)
		{
			out.push_back(*this);
			return out;
		}

		// If we get here, strSplitChars has been found in the string
		std::wstring strLine;
		std::wstring strAll = wideString;
		while (std::string::npos != pos)
		{
			// Copy character upto the position of the found strSplitChars into strLine
			strLine.assign(strAll, 0, pos);

			// Add the line to the output
			out.push_back(String(strLine));

			// Reset strLine
			strLine.clear();

			// Remove all characters including the strSplitChars from strAll
			strAll.erase(0, pos + strSplitChars.size());

			// Find next position of strSplitChars in strAll
			pos = strAll.find(strSplitChars.wideString, 0);
		}

		// If strAll still contains characters, add them to the vector
		if (strAll.size())
		{
			out.push_back(strAll);
		}
		return out;
	}

	std::string String::wideCharToMultiByte() const
	{
		if (wideString.empty())
			return std::string();
		int iSize = WideCharToMultiByte(CP_UTF8, 0, &wideString[0], (int)wideString.size(), NULL, 0, NULL, NULL);
		std::string strOut(iSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wideString[0], (int)wideString.size(), &strOut[0], iSize, NULL, NULL);
		return strOut;
	}

	void String::multiByteToWideChar(const std::string& multibyteString)
	{
		wideString.clear();
		if (multibyteString.empty())
			return;
		int iSize = MultiByteToWideChar(CP_UTF8, 0, &multibyteString[0], (int)multibyteString.size(), NULL, 0);
		std::wstring wstrResult(iSize, 0);
		MultiByteToWideChar(CP_UTF8, 0, &multibyteString[0], (int)multibyteString.size(), &wstrResult[0], iSize);
		wideString = wstrResult;
	}
}