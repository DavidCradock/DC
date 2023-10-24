#pragma once
#include "renderer.h"

namespace DC
{
	// Vulkan renderer
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

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
	private:
		class impl;
		impl* pimp;
	};
}