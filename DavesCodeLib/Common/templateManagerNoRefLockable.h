#pragma once
#include "string.h"
#include "error.h"
#include <map>

namespace DC
{
	// Template class to manage pointers to objects of a class of the given type.
	// This class has the ability to "lock" resources when they are added.
	// Locked objects are not removed upon the various remove methods unless they are passed true to their "forceRemoveLocked" parameter.
	// If you don't need this functionality, you can use the Manager or ManagerNoRef template classes instead.
	template <typename T> class ManagerNoRefLockable
	{
	public:
		// Constructor
		ManagerNoRefLockable()
		{
		}

		// Destructor
		~ManagerNoRefLockable()
		{
			removeAll(true);
		}

		// Adds a new uniquely named object.
		// If the named object already exists, this simply returns the object's pointer
		// If locked is set to true, this affects the various remove method's behaviours.
		// If calling a remove method on a named object which is set as locked, it will not
		// be removed unless that remove method is passed true to it's forceRemoveLocked parameter.
		// Objects can later be unlocked with a call to setLocked().
		T* add(const String& objectName, bool locked = false);

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
		// If the named object is set as locked, it will not be removed unless forceRemoveLocked is true.
		void remove(const String& objectName, bool forceRemoveLocked = false);

		// Removes all objects.
		// forceRemoveLocked, if true will remove objects regardless of their locked status, else, only unlocked resources are removed.
		void removeAll(bool forceRemoveLocked = false);

		// Sets the locked status of the named object.
		// If the named object doesn't exist, an exception occurs.
		void setLocked(const String& objectName, bool locked);
	private:
		struct SObject
		{
			T* object;
			bool locked;
		};
		std::map<String, SObject*> objects;
	};

	template <class T>
	T* ManagerNoRefLockable<T>::add(const String& objectName, bool locked)
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
		ErrorIfFalse(pNewObject, L"ManagerNoRefLockable::add(\"" + objectName + L"\") failed. Unable to allocate memory for new object.");
		pNewObject->locked = locked;
		pNewObject->object = new T;
		ErrorIfFalse(pNewObject->object, L"ManagerNoRefLockable::add(\"" + objectName + L"\") failed. Unable to allocate memory for new object.");
		objects[objectName] = pNewObject;
		return pNewObject->object;
	}

	template <class T>
	bool ManagerNoRefLockable<T>::exists(const String& objectName)
	{
		auto it = objects.find(objectName);
		return (objects.end() != it);
	}

	template <class T>
	T* ManagerNoRefLockable<T>::get(size_t index)
	{
		if (index < 0 || index >= objects.size())
		{
			String error = L"ManagerNoRefLockable::get(\"";
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
	T* ManagerNoRefLockable<T>::get(const String& objectName)
	{
		auto it = objects.find(objectName);
		ErrorIfTrue(objects.end() == it, L"ManagerNoRefLockable::get(\"" + objectName + L"\") failed. Object does not exist.");
		return it->second->object;
	}

	template <class T>
	String ManagerNoRefLockable<T>::getName(size_t index)
	{
		if (index < 0 || index >= objects.size())
		{
			String error = L"ManagerNoRefLockable::getName(\"";
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
	size_t ManagerNoRefLockable<T>::getNumber(void)
	{
		return objects.size();
	}

	template <class T>
	void ManagerNoRefLockable<T>::remove(const String& objectName, bool forceRemoveLocked)
	{
		// Find object
		auto it = objects.find(objectName);

		// If object doesn't exist, throw exception
		ErrorIfTrue(objects.end() == it, L"ManagerNoRefLockable::remove(\"" + objectName + L"\") failed. Object does not exist.");

		// If object is locked, simply return
		if (it->second->locked && forceRemoveLocked == false)
			return;

		// Remove object
		delete it->second->object;
		delete it->second;
		objects.erase(it);
	}

	template <class T>
	void ManagerNoRefLockable<T>::removeAll(bool forceRemoveLocked)
	{
		// Remove all objects
		if (forceRemoveLocked)
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

		// Go through map of objects, check locked status of each and get names for objects which should be removed
		std::vector<String> vecstrObjectsToDelete;
		auto it = objects.begin();
		while (objects.end() != it)
		{
			if (!it->second->locked)
			{
				vecstrObjectsToDelete.push_back(it->first);
			}
			it++;
		}

		// Remove each object name found
		for (size_t i = 0; i < vecstrObjectsToDelete.size(); i++)
			remove(vecstrObjectsToDelete[i]);
	}

	template <class T>
	void ManagerNoRefLockable<T>::setLocked(const String& objectName, bool locked)
	{
		// Find object and set locked status.
		auto it = objects.find(objectName);
		ErrorIfTrue(objects.end() == it, L"ManagerNoRefLockable::setLocked(\"" + objectName + L"\") failed. Named object does not exist.");
		it->second->locked = locked;
	}
}