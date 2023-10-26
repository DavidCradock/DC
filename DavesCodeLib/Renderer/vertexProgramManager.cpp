#include "vertexProgramManager.h"
#include "../Common/error.h"

namespace DC
{
	VertexProgramManager::VertexProgramManager()
	{
	}

	VertexProgramManager::~VertexProgramManager()
	{
		removeAll(true);
	}

	VertexProgram* VertexProgramManager::add(const String& objectName)
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
		pNewObject->object = new VertexProgram;
		ErrorIfMemoryNotAllocated(pNewObject->object);
		objects[objectName] = pNewObject;
		return pNewObject->object;
	}

	bool VertexProgramManager::exists(const String& objectName)
	{
		auto it = objects.find(objectName);
		return (objects.end() != it);
	}

	VertexProgram* VertexProgramManager::get(size_t index)
	{
		if (index < 0 || index >= objects.size())
		{
			String error = L"VertexProgramManager::get(\"";
			error.appendSizet(index);
			error += L"\") failed. Invalid index value given.";
			ErrorIfTrue(1, error);
		}
		auto it = objects.begin();
		for (size_t i = 0; i < index; i++)
			it++;
		return it->second->object;
	}

	VertexProgram* VertexProgramManager::get(const String& objectName)
	{
		auto it = objects.find(objectName);
		ErrorIfTrue(objects.end() == it, L"VertexProgramManager::get(\"" + objectName + L"\") failed. Object does not exist.");
		return it->second->object;
	}

	String VertexProgramManager::getName(size_t index)
	{
		if (index < 0 || index >= objects.size())
		{
			String error = L"VertexProgramManager::getName(\"";
			error.appendSizet(index);
			error += L"\") failed. Invalid index value given.";
			ErrorIfTrue(1, error);
		}
		auto it = objects.begin();
		for (size_t i = 0; i < index; i++)
			it++;
		return it->first;
	}

	size_t VertexProgramManager::getNumber(void)
	{
		return objects.size();
	}

	VertexProgram* VertexProgramManager::remove(const String& objectName)
	{
		auto it = objects.find(objectName);
		ErrorIfTrue(objects.end() == it, L"VertexProgramManager::remove(\"" + objectName + L"\") failed. Object does not exist.");
		it->second->referenceCount--;
		if (it->second->referenceCount > 0)
			return it->second->object;
		delete it->second->object;
		delete it->second;
		objects.erase(it);
		return 0;
	}

	void VertexProgramManager::removeAll(bool forceRemoval)
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