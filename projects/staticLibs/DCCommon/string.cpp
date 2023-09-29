#include "string.h"
#include <format>
#include <Windows.h>
#include <algorithm>
#include "error.h"

namespace DC
{
	void String::addFilenameExtension(const String& filenameExtension)
	{
		ErrorIfTrue(0 == filenameExtension.size(), L"String::addFilenameExtension() failed. Given extension name of zero length.");

		// Append "." to extension if needed
		String strExt = filenameExtension;
		if (strExt.c_str()[0] != '.')
		{
			std::wstring::iterator itBegin = strExt.begin();
			strExt.insert(itBegin, '.');
		}
		
		// Find last position of "." and remove everything after it
		auto const pos = this->find_last_of('.');
		if (pos != std::wstring::npos)
		{
			erase(pos, size() - pos);
		}

		append(strExt);
	}

	void String::appendDouble(double dValue, unsigned int uiNumDecimalPoints)
	{
		switch (uiNumDecimalPoints)
		{
		case 0:
			*this += std::format(L"{:.0f}", dValue);
			break;
		case 1:
			*this += std::format(L"{:.1f}", dValue);
			break;
		case 2:
			*this += std::format(L"{:.2f}", dValue);
			break;
		case 3:
			*this += std::format(L"{:.3f}", dValue);
			break;
		case 4:
			*this += std::format(L"{:.4f}", dValue);
			break;
		case 5:
			*this += std::format(L"{:.5f}", dValue);
			break;
		case 6:
			*this += std::format(L"{:.6f}", dValue);
			break;
		case 7:
			*this += std::format(L"{:.7f}", dValue);
			break;
		case 8:
			*this += std::format(L"{:.8f}", dValue);
			break;
		case 9:
			*this += std::format(L"{:.9f}", dValue);
			break;
		default:
			*this += std::format(L"{:.2f}", dValue);
		}
	}

	void String::appendFloat(float fValue, unsigned int uiNumDecimalPoints)
	{
		switch (uiNumDecimalPoints)
		{
		case 0:
			*this += std::format(L"{:.0f}", fValue);
			break;
		case 1:
			*this += std::format(L"{:.1f}", fValue);
			break;
		case 2:
			*this += std::format(L"{:.2f}", fValue);
			break;
		case 3:
			*this += std::format(L"{:.3f}", fValue);
			break;
		case 4:
			*this += std::format(L"{:.4f}", fValue);
			break;
		case 5:
			*this += std::format(L"{:.5f}", fValue);
			break;
		case 6:
			*this += std::format(L"{:.6f}", fValue);
			break;
		case 7:
			*this += std::format(L"{:.7f}", fValue);
			break;
		case 8:
			*this += std::format(L"{:.8f}", fValue);
			break;
		case 9:
			*this += std::format(L"{:.9f}", fValue);
			break;
		default:
			*this += std::format(L"{:.2f}", fValue);
		}
	}

	void String::appendInt(int iInt)
	{
		*this += std::to_wstring(iInt);
	}

	void String::appendUnsignedInt(unsigned int uiInt)
	{
		*this += std::to_wstring(uiInt);
	}

	void String::lowercase(void)
	{
		std::string str = wideCharToMultiByte();
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
		multiByteToWideChar(str);
	}

	bool String::representsNumber(void) const
	{
		return (std::all_of(begin(), end(), ::isdigit));
	}

	std::vector<String> String::splitString(const std::wstring& strSplitChars) const
	{
		std::vector<String> out;

		// If no strSplitChars found, simply add the entire string and return the result
		size_t pos = find(strSplitChars, 0);
		if (std::string::npos == pos)
		{
			out.push_back(*this);
			return out;
		}

		// If we get here, strSplitChars has been found in the string
		String strLine;
		String strAll = *this;
		while (std::string::npos != pos)
		{
			// Copy character upto the position of the found strSplitChars into strLine
			strLine.assign(strAll, 0, pos);

			// Add the line to the output
			out.push_back(strLine);

			// Reset strLine
			strLine.clear();

			// Remove all characters including the strSplitChars from strAll
			strAll.erase(0, pos + strSplitChars.size());

			// Find next position of strSplitChars in strAll
			pos = strAll.find(strSplitChars, 0);
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
		if (empty())
			return std::string();
		int iSize = WideCharToMultiByte(CP_UTF8, 0, &(*this)[0], (int)size(), NULL, 0, NULL, NULL);
		std::string strOut(iSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, &(*this)[0], (int)size(), &strOut[0], iSize, NULL, NULL);
		return strOut;
	}

	void String::multiByteToWideChar(const std::string& multibyteString)
	{
		clear();
		if (multibyteString.empty())
			return;
		int iSize = MultiByteToWideChar(CP_UTF8, 0, &multibyteString[0], (int)multibyteString.size(), NULL, 0);
		String wstrResult;
		wstrResult.reserve(iSize);
		MultiByteToWideChar(CP_UTF8, 0, &multibyteString[0], (int)multibyteString.size(), &wstrResult[0], iSize);
		*this = wstrResult;
	}
}