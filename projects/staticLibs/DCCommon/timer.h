#pragma once
#include "timerMinimal.h"
#include "string.h"

namespace DC
{
	// Timing
	// Create an object of this class, keep it around and call update() between the code which you wish to measure the time difference.
	class Timer
	{
	public:
		Timer();

		// Call this to compute time difference between last call to this method.
		void update(void);

		// Returns the time passed in seconds since the last call to the update() method.
		double getSecondsPast(void) const;

		// Suspends execution of the calling thread for the given number of milliseconds.
		// The sleep may be lengthened slightly by any system activity or by the time spent processing the call or by the granularity of system timers. 
		void sleep(unsigned int uiMilliseconds) const;

		// Sets the delay in seconds at which the getFPSAveraged() method's return value will be updated.
		void setAveragedFPSSecondsBetweenUpdates(double secondsBetweenUpdates);

		// Returns the currently set number of seconds between updates used by getFPSAveraged().
		double getAveragedFPSSecondsBetweenUpdates(void) const;

		// Returns computed Frames Per Second
		double getFramesPerSecond(void) const;

		// Returns computed Frames Per Second averaged value.
		double getFPSAveraged(void) const;

		// Returns the number of seconds until the next time the value returned by getFPSAveraged() is updated.
		double getFPSAveragedTimeUntilNextUpdate(void) const;

		// Resets all values to sane values.
		void reset(void);

		// Returns amount of seconds that we've been calling update()
		double getRuntimeSeconds(void) const;

		// Based on current runtime, sets parsed values to seconds, minutes, hours, days and weeks
		void getClock(double& seconds, int& minutes, int& hours, int& days, int& weeks) const;

		// Based on current runtime, returns current runtime as a string holding seconds, minutes, hours, days and weeks
		String getClock(void) const;
	private:
		TimerMinimal timer;
		double fps;
		double fpsAveraged;
		double fpsAveragedSecondsBetweenUpdates;
		double fpsAveragedTimeCount;
		int fpsAveragedNumCallsPerSec;
		double fpsAveragedAccum;
		double runtimeInSeconds;
	};
}