#include "rendererOpenGL.h"
#include "../Common/error.h"
#include "../Common/templateManagerNoRefLockable.h"
#include "../../third-party/SDL2-2.28.4/include/SDL.h"
#include "fragmentProgramOpenGL.h"
#include "texture2DOpenGL.h"
#include "vertexBufferOpenGL.h"
#include "vertexProgramOpenGL.h"
#include "map"

namespace DC
{
	class RendererOpenGL::impl
	{
	public:
		ManagerNoRefLockable<VertexBufferBase> vertexBuffers;
	};

	RendererOpenGL::RendererOpenGL()
	{
		pimp = new impl();
		ErrorIfMemoryNotAllocated(pimp);
	}

	RendererOpenGL::~RendererOpenGL()
	{
		delete pimp;
		pimp = 0;
	}

	void RendererOpenGL::init(void)
	{

	}

	void RendererOpenGL::shutdown(void)
	{

	}

	void RendererOpenGL::blendDisable(void)
	{

	}

	void RendererOpenGL::blendEnable(void)
	{

	}

	void RendererOpenGL::blendFunction_SrcAlpha_One(void)
	{

	}

	void RendererOpenGL::blendFunction_SrcAlpha_OneMinusSrcAlpha(void)
	{

	}

	void RendererOpenGL::cullFaceAntiClockwise(void)
	{

	}

	void RendererOpenGL::cullFaceClockwise(void)
	{

	}

	void RendererOpenGL::cullFaceDisable(void)
	{

	}

	void RendererOpenGL::cullFaceEnable(void)
	{

	}

	void RendererOpenGL::depthTestDisable(void)
	{

	}

	void RendererOpenGL::depthTestEnable(void)
	{

	}

	void RendererOpenGL::pixelsRead(int positionX, int positionY, int width, int height, void* pixels)
	{

	}

	void RendererOpenGL::pixelsWrite(int positionX, int positionY, int width, int height, void* pixels)
	{

	}

	void RendererOpenGL::scissorTest(int positionX, int positionY, int width, int height)
	{

	}

	void RendererOpenGL::scissorTestEnable(void)
	{

	}

	void RendererOpenGL::scissorTestDisable(void)
	{

	}

	VertexBufferBase* RendererOpenGL::vertexBufferAdd(const String& objectName, bool locked)
	{
		auto object = pimp->vertexBuffers.add(objectName, locked);
		return object;
	}

	bool RendererOpenGL::vertexBufferExists(const String& objectName)
	{
		return pimp->vertexBuffers.exists(objectName);
	}

	VertexBufferBase* RendererOpenGL::vertexBufferGet(size_t index)
	{
		return pimp->vertexBuffers.get(index);
	}

	VertexBufferBase* RendererOpenGL::vertexBufferGet(const String& objectName)
	{
		return pimp->vertexBuffers.get(objectName);
	}

	String RendererOpenGL::vertexBufferGetName(size_t index)
	{
		return pimp->vertexBuffers.getName(index);
	}

	size_t RendererOpenGL::vertexBufferGetNumber(void)
	{
		return pimp->vertexBuffers.getNumber();
	}

	void RendererOpenGL::vertexBufferRemove(const String& objectName, bool forceRemoveLocked)
	{
		pimp->vertexBuffers.remove(objectName, forceRemoveLocked);
	}

	void RendererOpenGL::vertexBufferRemoveAll(bool forceRemoveLocked)
	{
		pimp->vertexBuffers.removeAll(forceRemoveLocked);
	}

	void RendererOpenGL::vertexBufferSetLocked(const String& objectName, bool locked)
	{
		pimp->vertexBuffers.setLocked(objectName, locked);
	}
}