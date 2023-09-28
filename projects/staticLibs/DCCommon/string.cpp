#include "string.h"
#include <format>
#include <Windows.h>
#include <algorithm>

namespace DC
{

	String::String(void)
	{
		
	}

	String::String(const char* str)
	{
		string = str;
	}

	String::String(const std::string& str)
	{
		string = str;
	}

	void String::operator+=(const String& str)
	{
		string += str.string;
	}

	bool String::operator==(const String& str) const
	{
		return string == str.string;
	}

	String String::operator+(const String& str) const
	{
		String result;
		result.append(string);
		result.append(str);
		return result;
	}

	void String::append(const String& other)
	{
		string.append(other.string);
	}

	void String::append(const char* str)
	{
		string.append(str);
	}

	void String::appendDouble(double dValue, unsigned int uiNumDecimalPoints)
	{
		switch (uiNumDecimalPoints)
		{
		case 0:
			string += std::format("{:.0f}", dValue);
			break;
		case 1:
			string += std::format("{:.1f}", dValue);
			break;
		case 2:
			string += std::format("{:.2f}", dValue);
			break;
		case 3:
			string += std::format("{:.3f}", dValue);
			break;
		case 4:
			string += std::format("{:.4f}", dValue);
			break;
		case 5:
			string += std::format("{:.5f}", dValue);
			break;
		case 6:
			string += std::format("{:.6f}", dValue);
			break;
		case 7:
			string += std::format("{:.7f}", dValue);
			break;
		case 8:
			string += std::format("{:.8f}", dValue);
			break;
		case 9:
			string += std::format("{:.9f}", dValue);
			break;
		default:
			string += std::format("{:.2f}", dValue);
		}
	}

	void String::appendFloat(float fValue, unsigned int uiNumDecimalPoints)
	{
		switch (uiNumDecimalPoints)
		{
		case 0:
			string += std::format("{:.0f}", fValue);
			break;
		case 1:
			string += std::format("{:.1f}", fValue);
			break;
		case 2:
			string += std::format("{:.2f}", fValue);
			break;
		case 3:
			string += std::format("{:.3f}", fValue);
			break;
		case 4:
			string += std::format("{:.4f}", fValue);
			break;
		case 5:
			string += std::format("{:.5f}", fValue);
			break;
		case 6:
			string += std::format("{:.6f}", fValue);
			break;
		case 7:
			string += std::format("{:.7f}", fValue);
			break;
		case 8:
			string += std::format("{:.8f}", fValue);
			break;
		case 9:
			string += std::format("{:.9f}", fValue);
			break;
		default:
			string += std::format("{:.2f}", fValue);
		}
	}

	void String::appendInt(int iInt)
	{
		string += std::to_string(iInt);
	}

	void String::appendUnsignedInt(unsigned int uiInt)
	{
		string += std::to_string(uiInt);
	}

	const char* String::c_str(void) const
	{
		return string.c_str();
	}

	void String::clear(void)
	{
		string.clear();
	}

	void String::lowercase(void)
	{
		std::transform(string.begin(), string.end(), string.begin(), [](unsigned char c) { return std::tolower(c); });
	}

	bool String::representsNumber(void) const
	{
		return (std::all_of(string.begin(), string.end(), ::isdigit));
	}

	size_t String::size(void) const
	{
		return string.size();
	}

	std::vector<String> String::splitString(const String& strSplitChars) const
	{
		std::vector<String> out;

		// If no strSplitChars found, simply add the entire string and return the result
		size_t pos = string.find(strSplitChars.string, 0);
		if (std::string::npos == pos)
		{
			out.push_back(*this);
			return out;
		}

		// If we get here, strSplitChars has been found in the string
		std::string strLine;
		std::string strAll = string;
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
			pos = strAll.find(strSplitChars.string, 0);
		}

		// If strAll still contains characters, add them to the vector
		if (strAll.size())
		{
			out.push_back(strAll);
		}
		return out;
	}

	std::wstring String::wideGet() const
	{
		if (string.empty())
			return std::wstring();
		int iSize = MultiByteToWideChar(CP_UTF8, 0, &string[0], (int)string.size(), NULL, 0);
		std::wstring wstrOut(iSize, 0);
		MultiByteToWideChar(CP_UTF8, 0, &string[0], (int)string.size(), &wstrOut[0], iSize);
		return wstrOut;
	}

	void String::wideSet(const std::wstring& wstring)
	{
		string.clear();
		if (wstring.empty())
			return;
		int iSize = WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(), NULL, 0, NULL, NULL);
		std::string strOut(iSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(), &strOut[0], iSize, NULL, NULL);
		string = strOut;
	}
}