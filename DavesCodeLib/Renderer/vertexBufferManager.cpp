#include "vertexBufferManager.h"
#include "../Common/error.h"

namespace DC
{
	VertexBufferManager::VertexBufferManager()
	{
	}

	VertexBufferManager::~VertexBufferManager()
	{
		removeAll(true);
	}

	VertexBuffer* VertexBufferManager::add(const String& objectName)
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
		pNewObject->object = new VertexBuffer;
		ErrorIfMemoryNotAllocated(pNewObject->object);
		objects[objectName] = pNewObject;
		return pNewObject->object;
	}

	bool VertexBufferManager::exists(const String& objectName)
	{
		auto it = objects.find(objectName);
		return (objects.end() != it);
	}

	VertexBuffer* VertexBufferManager::get(size_t index)
	{
		if (index < 0 || index >= objects.size())
		{
			String error = L"VertexBufferManager::get(\"";
			error.appendSizet(index);
			error += L"\") failed. Invalid index value given.";
			ErrorIfTrue(1, error);
		}
		auto it = objects.begin();
		for (size_t i = 0; i < index; i++)
			it++;
		return it->second->object;
	}

	VertexBuffer* VertexBufferManager::get(const String& objectName)
	{
		auto it = objects.find(objectName);
		ErrorIfTrue(objects.end() == it, L"VertexBufferManager::get(\"" + objectName + L"\") failed. Object does not exist.");
		return it->second->object;
	}

	String VertexBufferManager::getName(size_t index)
	{
		if (index < 0 || index >= objects.size())
		{
			String error = L"VertexBufferManager::getName(\"";
			error.appendSizet(index);
			error += L"\") failed. Invalid index value given.";
			ErrorIfTrue(1, error);
		}
		auto it = objects.begin();
		for (size_t i = 0; i < index; i++)
			it++;
		return it->first;
	}

	size_t VertexBufferManager::getNumber(void)
	{
		return objects.size();
	}

	VertexBuffer* VertexBufferManager::remove(const String& objectName)
	{
		auto it = objects.find(objectName);
		ErrorIfTrue(objects.end() == it, L"VertexBufferManager::remove(\"" + objectName + L"\") failed. Object does not exist.");
		it->second->referenceCount--;
		if (it->second->referenceCount > 0)
			return it->second->object;
		delete it->second->object;
		delete it->second;
		objects.erase(it);
		return 0;
	}

	void VertexBufferManager::removeAll(bool forceRemoval)
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