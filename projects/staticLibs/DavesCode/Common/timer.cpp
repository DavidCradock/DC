#include "timer.h"
#include <Windows.h>

namespace DC
{
	Timer::Timer()
	{
		timer.reset();
		reset();
	}

	double Timer::getSecondsPast(void) const
	{
		return timer.getSecondsPast();
	}

	void Timer::update(void)
	{
		timer.update();
		double deltaTimeInSeconds = timer.getSecondsPast();

		// Compute FPS
		if (deltaTimeInSeconds > 0)
			fps = 1 / deltaTimeInSeconds;
		
		// Compute FPS smoothed
		fpsAveragedTimeCount += deltaTimeInSeconds;
		++fpsAveragedNumCallsPerSec;
		fpsAveragedAccum += fps;
		if (fpsAveragedTimeCount > fpsAveragedSecondsBetweenUpdates)
		{
			if (fpsAveragedNumCallsPerSec < 1)
				fpsAveragedNumCallsPerSec = 1;
			fpsAveraged = fpsAveragedAccum / fpsAveragedNumCallsPerSec;
			fpsAveragedTimeCount = 0;
			fpsAveragedNumCallsPerSec = 0;
			fpsAveragedAccum = 0;
		}

		// Runtime
		runtimeInSeconds += deltaTimeInSeconds;
	}

	void Timer::sleep(unsigned int milliseconds) const
	{
		Sleep(milliseconds);
	}

	void Timer::setAveragedFPSSecondsBetweenUpdates(double secondsBetweenUpdates)
	{
		fpsAveragedSecondsBetweenUpdates = secondsBetweenUpdates;
		if (fpsAveragedSecondsBetweenUpdates <= 0)
			fpsAveragedSecondsBetweenUpdates = 0.001;
	}

	double Timer::getAveragedFPSSecondsBetweenUpdates(void) const
	{
		return fpsAveragedSecondsBetweenUpdates;
	}

	double Timer::getFramesPerSecond(void) const
	{
		return fps;
	}

	double Timer::getFPSAveraged(void) const
	{
		return fpsAveraged;
	}

	double Timer::getFPSAveragedTimeUntilNextUpdate(void) const
	{
		return fpsAveragedSecondsBetweenUpdates - fpsAveragedTimeCount;
	}

	void Timer::reset(void)
	{
		timer.reset();

		// Stuff for FPS
		fps = 1;

		// Stuff for FPS averaged
		fpsAveraged = 1;
		fpsAveragedSecondsBetweenUpdates = 3;
		fpsAveragedTimeCount = 0;
		fpsAveragedNumCallsPerSec = 0;
		fpsAveragedAccum = 0;
		runtimeInSeconds = 0;
	}

	double Timer::getRuntimeSeconds(void) const
	{
		return runtimeInSeconds;
	}

	void Timer::getClock(double& seconds, int& minutes, int& hours, int& days, int& weeks) const
	{
		seconds = 0;
		minutes = 0;
		hours = 0;
		days = 0;
		weeks = 0;

		// How many seconds are in a week?
		// 60 seconds in a minute, 60 minutes in an hour, 24 hours in a day and 7 days in a week.
		// 60 * 60 = 3600 seconds per hour
		// 3600 * 24 = 86400 seconds in a day
		// 86400 * 7 = 604800 seconds in a week
		seconds = runtimeInSeconds;
		while (seconds >= 604800)   // Weeks
		{
			seconds -= 604800;
			weeks++;
		}
		while (seconds >= 86400)	// Days
		{
			seconds -= 86400;
			days++;
		}
		while (seconds >= 3600)		// Hours
		{
			seconds -= 3600;
			hours++;
		}
		while (seconds >= 60)		// Minutes
		{
			seconds -= 60;
			minutes++;
		}
	}

	String Timer::getClock(void) const
	{
		double secs = 0;
		int minutes, hours, days, weeks = 0;
		getClock(secs, minutes, hours, days, weeks);
		String str;
		str.appendInt((int)secs);
		str += L"sec ";
		str.appendInt(minutes);
		str += L"min ";
		str.appendInt(hours);
		str += L"hr ";
		str.appendInt(days);
		str += L"days ";
		str.appendInt(weeks);
		str += L"weeks.";
		return str;
	}
}