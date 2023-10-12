#pragma once
#include "string.h"

namespace DC
{

	// Converts the contents of a file into an array, stored inside a text header file, for inclusion of external files, inside the executable of a program
	bool convertFileToHeader(const String& filename, const String& arrayName = L"data", unsigned int numberOfElementsPerRow = 20);

	// Clamps a value to within the given values
	template <class type>
	inline void clamp(type& valueToClamp, type valueA, type valueB)
	{
		type min;
		type max;
		if (valueA < valueB)
		{
			min = valueA;
			max = valueB;
		}
		else
		{
			min = valueB;
			max = valueA;
		}
		if (valueToClamp < min)
			valueToClamp = min;
		else if (valueToClamp > max)
			valueToClamp = max;
	}


}