#pragma once
#include "applicationBase.h"
#include "../Common/timer.h"
#include "../Renderer/renderer.h"

namespace DC
{
	// Create an object of this class to handle initialisation, updating and shutting down of everything.
	// It ensures the correct order of initialisation of things and controls the main logic of the program.
	class Engine
	{
	public:
		Engine();

		// Pass a pointer to a class derived from ApplicationBase to initialise everything and begin the main loop.
		void go(ApplicationBase* pApplication);

		// Call this to exit the main loop and begin shutdown.
		void shutdown(void);
	private:
		// Application passed to go() method.
		ApplicationBase* application;

		Timer timer;

		// Set to true via shutdown()
		bool shutdownCalled;

		Renderer renderer;
	};
}