#pragma once

namespace DC
{
	// Base class which declares which methods need to be in a renderer class.
	// No implementation here
	class Renderer
	{
	public:

		virtual void blendDisable(void) = 0;
		virtual void blendEnable(void) = 0;
		virtual void blendFunction_SrcAlpha_One(void) = 0;
		virtual void blendFunction_SrcAlpha_OneMinusSrcAlpha(void) = 0;
		
		virtual void cullFaceAntiClockwise(void) = 0;
		virtual void cullFaceClockwise(void) = 0;
		virtual void cullFaceDisable(void) = 0;
		virtual void cullFaceEnable(void) = 0;

		virtual void depthTestDisable(void) = 0;
		virtual void depthTestEnable(void) = 0;
		
		virtual void pixelsRead(int positionX, int positionY, int width, int height, void* pixels) = 0;
		virtual void pixelsWrite(int positionX, int positionY, int width, int height, void* pixels) = 0;

		virtual void scissorTest(int positionX, int positionY, int width, int height) = 0;
		virtual void scissorTestDisable(void) = 0;
		virtual void scissorTestEnable(void) = 0;
	};
}