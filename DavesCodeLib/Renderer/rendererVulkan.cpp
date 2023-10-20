#include "rendererVulkan.h"
#include "../Common/error.h"
#include "../Common/templateManagerNoRefLockable.h"
#include "../../third-party/SDL2-2.28.4/include/SDL.h"
#include "fragmentProgramVulkan.h"
#include "texture2DVulkan.h"
#include "vertexBufferVulkan.h"
#include "vertexProgramVulkan.h"
#include <map>

namespace DC
{
	class RendererVulkan::impl
	{
	public:
		ManagerNoRefLockable<VertexBufferBase> vertexBuffers;
	};

	RendererVulkan::RendererVulkan()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	RendererVulkan::~RendererVulkan()
	{
		delete pimp;
		pimp = 0;
	}

	void RendererVulkan::init(void)
	{

	}

	void RendererVulkan::shutdown(void)
	{

	}

	void RendererVulkan::blendDisable(void)
	{

	}

	void RendererVulkan::blendEnable(void)
	{

	}

	void RendererVulkan::blendFunction_SrcAlpha_One(void)
	{

	}

	void RendererVulkan::blendFunction_SrcAlpha_OneMinusSrcAlpha(void)
	{

	}

	void RendererVulkan::cullFaceAntiClockwise(void)
	{

	}

	void RendererVulkan::cullFaceClockwise(void)
	{

	}

	void RendererVulkan::cullFaceDisable(void)
	{

	}

	void RendererVulkan::cullFaceEnable(void)
	{

	}

	void RendererVulkan::depthTestDisable(void)
	{

	}

	void RendererVulkan::depthTestEnable(void)
	{

	}

	void RendererVulkan::pixelsRead(int positionX, int positionY, int width, int height, void* pixels)
	{

	}

	void RendererVulkan::pixelsWrite(int positionX, int positionY, int width, int height, void* pixels)
	{

	}

	void RendererVulkan::scissorTest(int positionX, int positionY, int width, int height)
	{

	}

	void RendererVulkan::scissorTestEnable(void)
	{

	}

	void RendererVulkan::scissorTestDisable(void)
	{

	}

	VertexBufferBase* RendererVulkan::vertexBufferAdd(const String& objectName, bool locked)
	{
		auto object = pimp->vertexBuffers.add(objectName, locked);
		return object;
	}

	bool RendererVulkan::vertexBufferExists(const String& objectName)
	{
		return pimp->vertexBuffers.exists(objectName);
	}

	VertexBufferBase* RendererVulkan::vertexBufferGet(size_t index)
	{
		return pimp->vertexBuffers.get(index);
	}

	VertexBufferBase* RendererVulkan::vertexBufferGet(const String& objectName)
	{
		return pimp->vertexBuffers.get(objectName);
	}

	String RendererVulkan::vertexBufferGetName(size_t index)
	{
		return pimp->vertexBuffers.getName(index);
	}

	size_t RendererVulkan::vertexBufferGetNumber(void)
	{
		return pimp->vertexBuffers.getNumber();
	}

	void RendererVulkan::vertexBufferRemove(const String& objectName, bool forceRemoveLocked)
	{
		pimp->vertexBuffers.remove(objectName, forceRemoveLocked);
	}

	void RendererVulkan::vertexBufferRemoveAll(bool forceRemoveLocked)
	{
		pimp->vertexBuffers.removeAll(forceRemoveLocked);
	}

	void RendererVulkan::vertexBufferSetLocked(const String& objectName, bool locked)
	{
		pimp->vertexBuffers.setLocked(objectName, locked);
	}
}