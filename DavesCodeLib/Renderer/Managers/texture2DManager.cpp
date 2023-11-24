#include "texture2DManager.h"
#include "../../Common/error.h"

namespace DC
{
	Texture2DManager::Texture2DManager()
	{
	}

	Texture2DManager::~Texture2DManager()
	{
		removeAll(true);
	}

	Texture2D* Texture2DManager::add(const String& objectName)
	{
		auto it = objects.find(objectName);

		if (objects.end() != it)
		{
			it->second->referenceCount++;
			return it->second->object;
		}
		Object* pNewObject = new Object;
		ErrorIfMemoryNotAllocated(pNewObject);
		pNewObject->referenceCount = 1;
		pNewObject->object = new Texture2D;
		ErrorIfMemoryNotAllocated(pNewObject->object);
		objects[objectName] = pNewObject;
		return pNewObject->object;
	}

	bool Texture2DManager::exists(const String& objectName)
	{
		auto it = objects.find(objectName);
		return (objects.end() != it);
	}

	Texture2D* Texture2DManager::get(size_t index)
	{
		if (index < 0 || index >= objects.size())
		{
			String error = L"Texture2DManager::get(\"";
			error.appendSizet(index);
			error += L"\") failed. Invalid index value given.";
			ErrorIfTrue(1, error);
		}
		auto it = objects.begin();
		for (size_t i = 0; i < index; i++)
			it++;
		return it->second->object;
	}

	Texture2D* Texture2DManager::get(const String& objectName)
	{
		auto it = objects.find(objectName);
		ErrorIfTrue(objects.end() == it, L"Texture2DManager::get(\"" + objectName + L"\") failed. Object does not exist.");
		return it->second->object;
	}

	String Texture2DManager::getName(size_t index)
	{
		if (index < 0 || index >= objects.size())
		{
			String error = L"Texture2DManager::getName(\"";
			error.appendSizet(index);
			error += L"\") failed. Invalid index value given.";
			ErrorIfTrue(1, error);
		}
		auto it = objects.begin();
		for (size_t i = 0; i < index; i++)
			it++;
		return it->first;
	}

	size_t Texture2DManager::getNumber(void)
	{
		return objects.size();
	}

	Texture2D* Texture2DManager::remove(const String& objectName)
	{
		auto it = objects.find(objectName);
		ErrorIfTrue(objects.end() == it, L"Texture2DManager::remove(\"" + objectName + L"\") failed. Object does not exist.");
		it->second->referenceCount--;
		if (it->second->referenceCount > 0)
			return it->second->object;
		delete it->second->object;
		delete it->second;
		objects.erase(it);
		return 0;
	}

	void Texture2DManager::removeAll(bool forceRemoval)
	{
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
		std::vector<String> vecstrObjectsToDelete;
		auto it = objects.begin();
		while (objects.end() != it)
		{
			if (it->second->referenceCount == 1)
				vecstrObjectsToDelete.push_back(it->first);
			it++;
		}
		for (size_t i = 0; i < vecstrObjectsToDelete.size(); i++)
			remove(vecstrObjectsToDelete[i]);
	}
}