#pragma once
#include "rendererBase.h"

namespace DC
{
	class RendererVulkan : public Renderer
	{
	public:
		RendererVulkan();
		~RendererVulkan();

		void init(void);
		void shutdown(void);

		void blendDisable(void);
		void blendEnable(void);
		void blendFunction_SrcAlpha_One(void);
		void blendFunction_SrcAlpha_OneMinusSrcAlpha(void);

		void cullFaceAntiClockwise(void);
		void cullFaceClockwise(void);
		void cullFaceDisable(void);
		void cullFaceEnable(void);

		void depthTestDisable(void);
		void depthTestEnable(void);

		void pixelsRead(int positionX, int positionY, int width, int height, void* pixels);
		void pixelsWrite(int positionX, int positionY, int width, int height, void* pixels);

		void scissorTest(int positionX, int positionY, int width, int height);
		void scissorTestDisable(void);
		void scissorTestEnable(void);

		// Add a new, uniquely named vertex buffer object.
		// If the named object already exists, this simply returns the object's pointer
		// If locked is set to true, this affects the various remove method's behaviours.
		// If calling a remove method on a named object which is set as locked, it will not
		// be removed unless that remove method is passed true to it's forceRemoveLocked parameter.
		// Objects can later be unlocked with a call to setLocked().
		VertexBufferBase* vertexBufferAdd(const String& objectName, bool locked);

		// Returns whether the named vertex buffer exists or not
		bool vertexBufferExists(const String& objectName);

		// Returns a pointer to the vertex buffer at the specified index.
		// If the given index is invalid, an exception occurs.
		// Use getNumber() to determine valid index range.
		VertexBufferBase* vertexBufferGet(size_t index);

		// Attempts to find and return a pointer to the named vertex buffer.
		// If the named object could not be found, an exception occurs.
		// Use exists() to determine whether the object exists to prevent exception from occurring.
		VertexBufferBase* vertexBufferGet(const String& objectName);

		// Attempts to find and return the name of the vertex buffer at the specified index.
		// If the given index is invalid, an exception occurs.
		// Use getNumber() to determine valid index range.
		String vertexBufferGetName(size_t index);

		// Returns the number of vertex buffers.
		size_t vertexBufferGetNumber(void);

		// Attempts to remove the named vertex buffer.
		// If the named object does not exist, an exception occurs.
		// If the named object is set as locked, it will not be removed unless forceRemoveLocked is true.
		void vertexBufferRemove(const String& objectName, bool forceRemoveLocked = false);

		// Removes all vertex buffers.
		// forceRemoveLocked, if true will remove objects regardless of their locked status, else, only unlocked resources are removed.
		void vertexBufferRemoveAll(bool forceRemoveLocked = false);

		// Sets the locked status of the named vertex buffer.
		// If the named object doesn't exist, an exception occurs.
		void vertexBufferSetLocked(const String& objectName, bool locked);
	private:
		class impl;
		impl* pimp;
	};
}