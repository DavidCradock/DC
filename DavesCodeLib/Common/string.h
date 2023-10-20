#pragma once
#include <string>
#include <vector>
#include <fstream>

namespace DC
{
	// Wide string unicode object to store and manipulate strings.
	// I've chosen wide strings because multibyte are difficult to work with and these wide strings support characters for many languages and
	// if a user has special non latin ASCII characters in either their name, keyboard input, filesystems etc, they will not be able to use my
	// programs properly, if at all.
	// Only around 36% of the world's population use (to me) standard latin character set.
	// 18% use Chinese, 14% Dewanagari, another 14% Arabic, 4% Cyrillic and another 4% Dravidian.
	// When adding strings, we have to pre-pend the quotation marks with a capital l, L.
	// This is the only downside that I can think of, to using wide strings.
	// To insert Unicode characters we have to escape the 4-hexadecimal-character representation with an \u.
	// We can find these on, for example, Wikipedia over at https://en.wikipedia.org/wiki/List_of_Unicode_characters
	// For example...
	// String str;
	// str += L"\u2211" would add the mathematical symbol for Sigma.
	class String : public std::wstring
	{
	public:
		String();

		String(const wchar_t* string);

		// Assumes that this string already contains a filename and then appends the given filename extension to the end of this string.
		// If this string already has an extension, it is removed and replaced with the new one.
		// The filename extension may or may not have the "." at the beginning. If it doesn't exist, it is added.
		// If the filename extension is of zero length, an exception occurs.
		void addFilenameExtension(const String& filenameExtension);

		// Append a wchar_t* to the end of this string
		void appendWChar(const wchar_t* string);

		// Append a double to the end of this string with the given number of digits after the decimal point.
		// numDecimalPoints can be from 0 to 9, if outside this range, then 2 is used.
		void appendDouble(double value, unsigned int numDecimalPoints = 2);

		// Append a float to the end of this string with the given number of digits after the decimal point.
		// numDecimalPoints can be from 0 to 9, if outside this range, then 2 is used.
		void appendFloat(float value, unsigned int numDecimalPoints = 2);

		// Append an integer to the end of this string
		void appendInt(int value);

		// Append a size_t to the end of this string
		void appendSizet(size_t value);

		// Append an unsigned integer to the end of this string
		void appendUnsignedInt(unsigned int value);

		// Append a const char pointer
		void appendCharPointer(const char* pCharacters);

		// Append a std::string
		void appendString(const std::string& stringToAppend);

		// Return a pointer to a narrow c_string
		// NOTE: Uses c_strTemp as a temp member variable so the returned pointer is valid and non-dangling.
		// TODO While at work, think of a better way to do this.
		const char* c_strn(void) const;

		// Return a pointer to a c_string
		const wchar_t* c_strw(void);

		// Converts this string to lowercase
		void lowercase(void);

		// Converts this string to uppercase
		void uppercase(void);

		// Returns true if the string has nothing but digits
		bool representsNumber(void) const;

		// Finds each occurrence of strSplitChars and returns a vector of strings which
		// contains strings that are either side of the strSplitChars, with the strSplitChars removed.
		// This is usefull for splitting text into multiple lines, using a specified character sequence used to
		// represent the end of the line of text.
		// For example, if the string is "Line1\nLine2\nLine3" and the strSplitChars is "\n"
		// then the returned vector would have 3 entries, "Line1" "Line2" and "Line3"
		std::vector<String> splitString(const std::wstring& splitChars = L"\n") const;

		// Returns this string as a narrow/multibyte string
		std::string wideCharToMultiByte() const;

		// Sets this string from a narrow/multibyte string
		void multiByteToWideChar(const std::string& multibyteString);

		// Writes out this string to an already opened ofstream.
		// The writes out the size of the string and then the string itself.
		// If the file is not open, an exception occurs
		void ofstreamWrite(std::ofstream& file);

		// Reads in a string from an already opened ifstream.
		// This reads in the size of the string, resizes it to make room and then loads it in
		// If the file is not open or there was an error during reading, an exception occurs.
		void ifstreamRead(std::ifstream& file);
	private:

		// Used by c_str() to return a pointer
		mutable std::string c_strTemp;
	};
}