#pragma once

namespace DC
{
	class Framework;

	// A base class to derive a new class from to use with the framework manager class.
	// We create our own application class from this one and implement the various methods we need.
	// One method, onUpdate() has to be implemented as it's pure virtual, the others, we can choose if we need them or not.
	// Usage...
	// We inherit a new class from this one, implementing the methods we need and then pass a pointer to an object of this new class
	// to the frame work manager class via it's go() method.
	class ApplicationBase
	{
	public:
		// Pure virtual. Needs to be overridden.
		// Gets called at the beginning of the application's main loop.
		virtual void onUpdate(double timeInSecondsSinceLastCall, Framework* pFramework) = 0;

		// Virtual. Implement if needed.
		// Gets called just before the framework manager enters the main program loop,
		// after everything has been initialised and ready to go.
		virtual void onInit(void);

		// Virtual. Implement if needed.
		// Gets called just after the framework manager has exited the main program loop,
		// before everything else gets cleaned up and shutdown.
		virtual void onShutdown(void);
	};
}