#pragma once
#include "fragmentProgram.h"

namespace DC
{
	// Manager class to handle fragment programs.
	// The renderer manager class has an object of this class ready to use.
	class FragmentProgramManager
	{
	public:
		// Adds a new uniquely named object.
		// If the named object already exists, it's reference count is increased.
		FragmentProgram* add(const String& objectName);

		// Returns whether the named object exists or not
		bool exists(const String& objectName);

		// Returns a pointer to the object at the specified index.
		// If the given index is invalid, an exception occurs.
		// Use getNumber() to determine valid index range.
		FragmentProgram* get(size_t index);

		// Attempts to find and return a pointer to the named object.
		// If the named object could not be found, an exception occurs.
		// Use exists() to determine whether the object exists to prevent exception from occurring.
		FragmentProgram* get(const String& objectName);

		// Attempts to find and return the name of the object at the specified index.
		// If the given index is invalid, an exception occurs.
		// Use getNumber() to determine valid index range.
		String getName(size_t index);

		// Returns the number of objects.
		size_t getNumber(void);

		// Attempts to remove the named object.
		// If the named object does not exist, an exception occurs.
		// If the named object exists and has been added multiple times, it's reference count is decreased and the object remains.
		void remove(const String& objectName);

		// Removes all objects.
		// forceRemoval, if true will remove objects regardless of their reference count, else only objects with a reference count of 0 will be removed.
		void removeAll(bool forceRemoval = false);
	};
}