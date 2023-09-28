#pragma once
#include <string>
#include <vector>

namespace DC
{

	class String
	{
	public:
		// Default constructor
		String();

		// Constructor accepting a pointer to chars
		String(const char* str);

		// Constructor accepting std::string
		String(const std::string& str);

		// += operator overload to append strings
		void operator+=(const String& str);

		// == operator overload for comparison
		bool operator==(const String& str) const;

		// + operator overload for appending a string
		String operator+(const String& str) const;

		// Append the given string to the end of this one
		void append(const String& other);

		// Append the given string to the end of this one
		void append(const char* str);

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

		const char* c_str(void) const;

		// Clear the string
		void clear(void);

		// Converts this string to lowercase
		void lowercase(void);

		// Returns true if the string has nothing but digits
		bool representsNumber(void) const;

		// Returns the size of this string
		size_t size(void) const;

		// Finds each occurrence of strSplitChars and returns a vector of strings which
		// contains strings that are either side of the strSplitChars, with the strSplitChars removed.
		// This is usefull for splitting text into multiple lines, using a specified character sequence used to
		// represent the end of the line of text.
		// For example, if the string is "Line1\nLine2\nLine3" and the strSplitChars is "\n"
		// then the returned vector would have 3 entries, "Line1" "Line2" and "Line3"
		std::vector<String> splitString(const String& strSplitChars = "\n") const;

		// Returns this string as a wide string
		std::wstring wideGet() const;

		// Sets this string from a wide string
		void wideSet(const std::wstring& wstring);

	private:
		std::string string;
	};
}