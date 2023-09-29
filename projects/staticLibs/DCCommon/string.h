#pragma once
#include <string>
#include <vector>

namespace DC
{

	class String : public std::wstring
	{
	public:
		// Assumes that this string already contains a filename and then appends the given filename extension to the end of this string.
		// If this string already has an extension, it is removed and replaced with the new one.
		// The filename extension may or may not have the "." at the beginning. If it doesn't exist, it is added.
		// If the filename extension is of zero length, an exception occurs.
		void addFilenameExtension(const String& filenameExtension);

		// Append a double to the end of this string with the given number of digits after the decimal point.
		// uiNumDecimalPoints can be from 0 to 9, if outside this range, then 2 is used.
		void appendDouble(double dValue, unsigned int uiNumDecimalPoints = 2);

		// Append a float to the end of this string with the given number of digits after the decimal point.
		// uiNumDecimalPoints can be from 0 to 9, if outside this range, then 2 is used.
		void appendFloat(float fValue, unsigned int uiNumDecimalPoints = 2);

		// Append an integer to the end of this string
		void appendInt(int iInt);

		// Append an unsigned integer to the end of this string
		void appendUnsignedInt(unsigned int uiInt);

		// Converts this string to lowercase
		void lowercase(void);

		// Returns true if the string has nothing but digits
		bool representsNumber(void) const;

		// Finds each occurrence of strSplitChars and returns a vector of strings which
		// contains strings that are either side of the strSplitChars, with the strSplitChars removed.
		// This is usefull for splitting text into multiple lines, using a specified character sequence used to
		// represent the end of the line of text.
		// For example, if the string is "Line1\nLine2\nLine3" and the strSplitChars is "\n"
		// then the returned vector would have 3 entries, "Line1" "Line2" and "Line3"
		std::vector<String> splitString(const std::wstring& strSplitChars = L"\n") const;

		// Returns this string as a narrow/multibyte string
		std::string wideCharToMultiByte() const;

		// Sets this string from a narrow/multibyte string
		void multiByteToWideChar(const std::string& multibyteString);
	};
}