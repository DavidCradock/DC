#pragma once
#include <chrono>

namespace DC
{
	// Timing
	// Create an object of this class, keep it around and call update() between the code which you wish to measure the time difference.
	// Then call getSecondsPast() to obtain the time passed since the two calls to update()
	// This class is similar to Timer, but with less features and is slightly faster to compute.
	class TimerMinimal
	{
	public:
		TimerMinimal();

		// Call this to compute time difference between last call to this method.
		void update(void);

		// Returns time passed in seconds since last call to update() method.
		double getSecondsPast(void) const;

		// Resets all values to sane values
		void reset(void);

	private:
		std::chrono::duration<double> timeDeltaSec;
		std::chrono::time_point<std::chrono::steady_clock> timePointOld, timePointNew;
		double deltaSecondsBetweenCallsToUpdate;
	};
}