#pragma once
#include <string>

class DCString : public std::string
{
public:
	
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

};