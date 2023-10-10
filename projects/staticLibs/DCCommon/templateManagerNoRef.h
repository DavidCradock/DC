#pragma once
#include "string.h"
#include "error.h"
#include <map>

namespace DC
{
	// Template class to manage pointers to objects of a class of the given type.
	template <typename T> class ManagerNoRef
	{
	public:
		// Constructor
		ManagerNoRef()
		{
		}

		// Destructor
		~ManagerNoRef()
		{
			removeAll();
		}

		// Adds a new uniquely named object.
		// If the named object already exists, this simply returns the object's pointer
		T* add(const String& objectName);

		// Attempts to remove the named object.
		// If the named object does not exist, an exception occurs.
		void remove(const String& objectName);

		// Returns whether the named object exists or not
		bool exists(const String& objectName);

		// Removes all objects.
		void removeAll(void);

		// Returns the number of objects.
		size_t getNumber(void);

		// Attempts to find and return the name of the object at the specified index.
		// If the given index is invalid, an exception occurs.
		// Use getNumber() to determine valid index range.
		String getName(size_t index);

		// Returns a pointer to the object at the specified index.
		// If the given index is invalid, an exception occurs.
		// Use getNumber() to determine valid index range.
		T* get(size_t index);

		// Attempts to find and return a pointer to the named object.
		// If the named object could not be found, an exception occurs.
		// Use exists() to determine whether the object exists to prevent exception from occurring.
		T* get(const String& objectName);
	private:
		struct SObject
		{
			T* object;
		};
		std::map<String, SObject*> objects;
	};

	template <class T>
	T* ManagerNoRef<T>::add(const String& objectName)
	{
		// Find object
		auto it = objects.find(objectName);

		// If object exists, return object
		if (objects.end() != it)
		{
			return it->second->object;
		}

		// Create new object and return it
		SObject* pNewObject = new SObject;
		ErrorIfFalse(pNewObject, L"ManagerNoRef::add(\"" + objectName + L"\") failed. Unable to allocate memory for new object.");
		pNewObject->object = new T;
		ErrorIfFalse(pNewObject->object, L"ManagerNoRef::add(\"" + objectName + L"\") failed. Unable to allocate memory for new object.");
		objects[objectName] = pNewObject;
		return pNewObject->object;
	}

	template <class T>
	void ManagerNoRef<T>::remove(const String& objectName)
	{
		// Find object
		auto it = objects.find(objectName);

		// If object doesn't exist, throw exception
		ErrorIfTrue(objects.end() == it, L"ManagerNoRef::remove(\"" + objectName + L"\") failed. Object does not exist.");

		// Remove object
		delete it->second->object;
		delete it->second;
		objects.erase(it);
	}

	template <class T>
	bool ManagerNoRef<T>::exists(const String& objectName)
	{
		auto it = objects.find(objectName);
		return (objects.end() != it);
	}

	template <class T>
	void ManagerNoRef<T>::removeAll(void)
	{
		// Remove all objects
		auto it = objects.begin();
		while (it != objects.end())
		{
			delete it->second->object;
			delete it->second;
			objects.erase(it);
			it = objects.begin();
		}
	}

	template <class T>
	size_t ManagerNoRef<T>::getNumber(void)
	{
		return objects.size();
	}

	template <class T>
	String ManagerNoRef<T>::getName(size_t index)
	{
		if (index < 0 || index >= objects.size())
		{
			String error = L"ManagerNoRef::getName(\"";
			error.appendSizet(index);
			error += L"\") failed. Invalid index value given.";
			ErrorIfTrue(1, error);
		}

		auto it = objects.begin();
		for (size_t i = 0; i < index; i++)
		{
			it++;
		}
		return it->first;
	}

	template <class T>
	T* ManagerNoRef<T>::get(size_t index)
	{
		if (index < 0 || index >= objects.size())
		{
			String error = L"ManagerNoRef::get(\"";
			error.appendSizet(index);
			error += L"\") failed. Invalid index value given.";
			ErrorIfTrue(1, error);
		}

		auto it = objects.begin();
		for (size_t i = 0; i < index; i++)
		{
			it++;
		}
		return it->second->object;
	}

	template <class T>
	T* ManagerNoRef<T>::get(const String& objectName)
	{
		auto it = objects.find(objectName);
		ErrorIfTrue(objects.end() == it, L"ManagerNoRef::get(\"" + objectName + L"\") failed. Object does not exist.");
		return it->second->object;
	}
}