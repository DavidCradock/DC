#include "timerMinimal.h"

namespace DC
{
	TimerMinimal::TimerMinimal()
	{
		reset();
	}

	double TimerMinimal::getSecondsPast(void) const
	{
		return deltaSecondsBetweenCallsToUpdate;
	}

	void TimerMinimal::update(void)
	{
		timePointNew = std::chrono::steady_clock::now();
		timeDeltaSec = timePointNew - timePointOld;
		timePointOld = timePointNew;
		deltaSecondsBetweenCallsToUpdate = timeDeltaSec.count();
	}

	void TimerMinimal::reset(void)
	{
		timePointNew = std::chrono::steady_clock::now();
		timePointOld = timePointNew;
		timeDeltaSec = timePointNew - timePointOld;
		deltaSecondsBetweenCallsToUpdate = timeDeltaSec.count();
	}
}