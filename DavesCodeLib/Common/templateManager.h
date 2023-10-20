#pragma once
#include "string.h"
#include "error.h"
#include <map>

namespace DC
{
	// Template class to manage pointers to objects of a class of the given type.
	// An object has a reference count, so if it is added multiple times, it will not be removed with calls to remove() until the reference count has reached 0.
	// That is unless removeAll() is passed true to it's forceRemoval parameter in which case, it is.
	// If you do not wish to mess around with references, use the ManagerNoRef template class instead.
	template <typename T> class Manager
	{
	public:
		// Constructor
		Manager()
		{
		}

		// Destructor
		~Manager()
		{
			removeAll(true);
		}

		// Adds a new uniquely named object.
		// If the named object already exists, it's reference count is increased.
		T* add(const String& objectName);

		// Returns whether the named object exists or not
		bool exists(const String& objectName);

		// Returns a pointer to the object at the specified index.
		// If the given index is invalid, an exception occurs.
		// Use getNumber() to determine valid index range.
		T* get(size_t index);

		// Attempts to find and return a pointer to the named object.
		// If the named object could not be found, an exception occurs.
		// Use exists() to determine whether the object exists to prevent exception from occurring.
		T* get(const String& objectName);

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
	private:
		struct SObject
		{
			T* object;
			unsigned int referenceCount;
		};
		std::map<String, SObject*> objects;
	};

	template <class T>
	T* Manager<T>::add(const String& objectName)
	{
		// Find object
		auto it = objects.find(objectName);

		// If object exists, increment reference count and return object
		if (objects.end() != it)
		{
			it->second->referenceCount++;
			return it->second->object;
		}

		// Create new object and return it
		SObject* pNewObject = new SObject;
		ErrorIfFalse(pNewObject, L"Manager::add(\"" + objectName + L"\") failed. Unable to allocate memory for new object.");
		pNewObject->referenceCount = 1;
		pNewObject->object = new T;
		ErrorIfFalse(pNewObject->object, L"Manager::add(\"" + objectName + L"\") failed. Unable to allocate memory for new object.");
		objects[objectName] = pNewObject;
		return pNewObject->object;
	}

	template <class T>
	bool Manager<T>::exists(const String& objectName)
	{
		auto it = objects.find(objectName);
		return (objects.end() != it);
	}

	template <class T>
	T* Manager<T>::get(size_t index)
	{
		if (index < 0 || index >= objects.size())
		{
			String error = L"Manager::get(\"";
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
	T* Manager<T>::get(const String& objectName)
	{
		auto it = objects.find(objectName);
		ErrorIfTrue(objects.end() == it, L"Manager::get(\"" + objectName + L"\") failed. Object does not exist.");
		return it->second->object;
	}

	template <class T>
	String Manager<T>::getName(size_t index)
	{
		if (index < 0 || index >= objects.size())
		{
			String error = L"Manager::getName(\"";
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
	size_t Manager<T>::getNumber(void)
	{
		return objects.size();
	}

	template <class T>
	void Manager<T>::remove(const String& objectName)
	{
		// Find object
		auto it = objects.find(objectName);

		// If object doesn't exist, throw exception
		ErrorIfTrue(objects.end() == it, L"Manager::remove(\"" + objectName + L"\") failed. Object does not exist.");

		// Decrement object's reference count
		it->second->referenceCount--;

		// If reference count is still high, simply return
		if (it->second->referenceCount > 0)
			return;

		// Remove object
		delete it->second->object;
		delete it->second;
		objects.erase(it);
	}

	template <class T>
	void Manager<T>::removeAll(bool forceRemoval)
	{
		// Remove all objects
		if (forceRemoval)
		{
			auto it = objects.begin();
			while (it != objects.end())
			{
				delete it->second->object;
				delete it->second;
				objects.erase(it);
				it = objects.begin();
			}
			return;
		}

		// Go through map of objects, decrease reference count of each and get names for objects which should be removed
		std::vector<String> vecstrObjectsToDelete;
		auto it = objects.begin();
		while (objects.end() != it)
		{
			if (it->second->referenceCount == 1)
			{
				vecstrObjectsToDelete.push_back(it->first);
			}
			it++;
		}

		// Remove each object name found
		for (size_t i = 0; i < vecstrObjectsToDelete.size(); i++)
			remove(vecstrObjectsToDelete[i]);
	}
}