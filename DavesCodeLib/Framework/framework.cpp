#include "framework.h"
#include "../Common/error.h"
#include "../../third-party/SDL2-2.28.4/include/SDL.h"

namespace DC
{
	Framework::Framework()
	{
		application = 0;
		shutdownCalled = false;
	}

	void Framework::go(ApplicationBase* pApplication)
	{
		ErrorIfTrue(application, L"Framework::go() failed. go already called as existing application pointer is not 0.");
		application = pApplication;

		initialiseThirdparty();

		application->onInit();

		while (!shutdownCalled)
		{
			timer.update();
			application->onUpdate(timer.getSecondsPast(), this);
		}

		application->onShutdown();

		shutdownThirdparty();
	}

	void Framework::shutdown(void)
	{
		shutdownCalled = true;
	}

	void Framework::initialiseThirdparty(void)
	{
		if (0 != SDL_InitSubSystem(SDL_INIT_EVERYTHING))
		{
			String error;
			error.appendCharPointer(SDL_GetError());
			ErrorIfTrue(1, error);
		}
	}

	void Framework::shutdownThirdparty(void)
	{

	}
}