#pragma once
#include "texture2D.h"
#include <map>

namespace DC
{
	// Manager class to handle texture2D objects.
	// The Renderer class has a public object of this class ready to use.
	// Objects are reference counted.
	class Texture2DManager
	{
	public:
		// Constructor
		Texture2DManager();

		// Destructor
		~Texture2DManager();

		// Adds a new uniquely named object.
		// If the named object already exists, it's reference count is increased and the existing object is returned.
		Texture2D* add(const String& objectName);

		// Returns whether the named object exists or not
		bool exists(const String& objectName);

		// Returns a pointer to the object at the specified index.
		// If the given index is invalid, an exception occurs.
		// Use getNumber() to determine valid index range.
		Texture2D* get(size_t index);

		// Attempts to find and return a pointer to the named object.
		// If the named object could not be found, an exception occurs.
		// Use exists() to determine whether the object exists to prevent exception from occurring.
		Texture2D* get(const String& objectName);

		// Attempts to find and return the name of the object at the specified index.
		// If the given index is invalid, an exception occurs.
		// Use getNumber() to determine valid index range.
		String getName(size_t index);

		// Returns the number of objects currently added with add().
		size_t getNumber(void);

		// Attempts to remove the named object.
		// If the named object does not exist, an exception occurs.
		// If the named object exists and has been added multiple times, it's reference count is decreased and the object remains and it's pointer is returned, else 0.
		Texture2D* remove(const String& objectName);

		// Removes all objects.
		// forceRemoval, if true will remove objects regardless of their reference count, else only objects with a reference count of 1 will be removed.
		void removeAll(bool forceRemoval = false);

	private:
		struct Object
		{
			Texture2D* object;
			unsigned int referenceCount;
		};
		std::map<String, Object*> objects;
	};
}