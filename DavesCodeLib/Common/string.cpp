#include "string.h"
#include <format>
#include <Windows.h>
#include <algorithm>
#include "error.h"

namespace DC
{
	String::String()
	{

	}

	String::String(const wchar_t* string)
	{
		this->append(string);
	}

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

	void String::appendWChar(const wchar_t* string)
	{
		this->append(string);
	}

	void String::appendDouble(double value, unsigned int numDecimalPoints)
	{
		switch (numDecimalPoints)
		{
		case 0:
			*this += std::format(L"{:.0f}", value);
			break;
		case 1:
			*this += std::format(L"{:.1f}", value);
			break;
		case 2:
			*this += std::format(L"{:.2f}", value);
			break;
		case 3:
			*this += std::format(L"{:.3f}", value);
			break;
		case 4:
			*this += std::format(L"{:.4f}", value);
			break;
		case 5:
			*this += std::format(L"{:.5f}", value);
			break;
		case 6:
			*this += std::format(L"{:.6f}", value);
			break;
		case 7:
			*this += std::format(L"{:.7f}", value);
			break;
		case 8:
			*this += std::format(L"{:.8f}", value);
			break;
		case 9:
			*this += std::format(L"{:.9f}", value);
			break;
		default:
			*this += std::format(L"{:.2f}", value);
		}
	}

	void String::appendFloat(float value, unsigned int numDecimalPoints)
	{
		switch (numDecimalPoints)
		{
		case 0:
			*this += std::format(L"{:.0f}", value);
			break;
		case 1:
			*this += std::format(L"{:.1f}", value);
			break;
		case 2:
			*this += std::format(L"{:.2f}", value);
			break;
		case 3:
			*this += std::format(L"{:.3f}", value);
			break;
		case 4:
			*this += std::format(L"{:.4f}", value);
			break;
		case 5:
			*this += std::format(L"{:.5f}", value);
			break;
		case 6:
			*this += std::format(L"{:.6f}", value);
			break;
		case 7:
			*this += std::format(L"{:.7f}", value);
			break;
		case 8:
			*this += std::format(L"{:.8f}", value);
			break;
		case 9:
			*this += std::format(L"{:.9f}", value);
			break;
		default:
			*this += std::format(L"{:.2f}", value);
		}
	}

	void String::appendInt(int value)
	{
		*this += std::to_wstring(value);
	}

	void String::appendSizet(size_t value)
	{
		*this += std::to_wstring(value);
	}

	void String::appendUnsignedInt(unsigned int value)
	{
		*this += std::to_wstring(value);
	}

	void String::appendCharPointer(const char* pCharacters)
	{
		std::string strMB = pCharacters;
		this->appendString(strMB);
	}

	void String::appendString(const std::string& stringToAppend)
	{
		String strTemp;
		strTemp.multiByteToWideChar(stringToAppend);
		this->append(strTemp);
	}

	const char* String::c_strn(void) const
	{
		c_strTemp = wideCharToMultiByte();
		return c_strTemp.c_str();
	}

	const wchar_t* String::c_strw(void)
	{
		return c_str();
	}

	void String::lowercase(void)
	{
		std::string str = wideCharToMultiByte();
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
		multiByteToWideChar(str);
	}

	void String::uppercase(void)
	{
		std::string str = wideCharToMultiByte();
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
		multiByteToWideChar(str);
	}

	bool String::representsNumber(void) const
	{
		return (std::all_of(begin(), end(), ::isdigit));
	}

	std::vector<String> String::splitString(const std::wstring& splitChars) const
	{
		std::vector<String> out;

		// If no splitChars found, simply add the entire string and return the result
		size_t pos = find(splitChars, 0);
		if (std::string::npos == pos)
		{
			out.push_back(*this);
			return out;
		}

		// If we get here, splitChars has been found in the string
		String strLine;
		String strAll = *this;
		while (std::string::npos != pos)
		{
			// Copy character upto the position of the found splitChars into strLine
			strLine.assign(strAll, 0, pos);

			// Add the line to the output
			out.push_back(strLine);

			// Reset strLine
			strLine.clear();

			// Remove all characters including the splitChars from strAll
			strAll.erase(0, pos + splitChars.size());

			// Find next position of splitChars in strAll
			pos = strAll.find(splitChars, 0);
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
		std::wstring wstrResult(iSize, 0);
		MultiByteToWideChar(CP_UTF8, 0, &multibyteString[0], (int)multibyteString.size(), &wstrResult[0], iSize);
		append(wstrResult.c_str());
	}

	void String::ofstreamWrite(std::ofstream& file)
	{
		ErrorIfFalse(file.is_open(), L"String::ofstreamWrite() failed. The given ofstream is not open.");
		size_t size = this->size();
		file.write((char*)&size, sizeof(size));
		file.write(c_strn(), size);
		ErrorIfFalse(file.good(), L"String::ofstreamWrite() failed. The ofstream is not good.");
	}

	void String::ifstreamRead(std::ifstream& file)
	{
		ErrorIfFalse(file.is_open(), L"String::ifstreamRead() failed. The given ifstream is not open.");
		clear();
		size_t size;
		file.read((char*)&size, sizeof(size));
		resize(size);
		//file.read(&strString[0], size);
		file.read((char*)c_str(), size);
		ErrorIfFalse(file.good(), L"String::ifstreamRead() failed. The ifstream is not good.");
	}
}