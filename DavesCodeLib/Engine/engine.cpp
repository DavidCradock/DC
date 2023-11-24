#include "engine.h"
#include "../Common/error.h"

namespace DC
{
	Engine::Engine()
	{
		application = 0;
		shutdownCalled = false;
	}

	void Engine::go(ApplicationBase* pApplication)
	{
		ErrorIfTrue(application, L"Engine::go() failed. go already called as existing application pointer is not 0.");
		application = pApplication;

		settings.load();

		renderer.init(settings);

		application->onInit();

		while (!shutdownCalled)
		{
			timer.update();
			application->onUpdate(timer.getSecondsPast(), this);
			if (!renderer.update())
				shutdownCalled = true;
		}

		application->onShutdown();

		renderer.shutdown();


	}

	void Engine::shutdown(void)
	{
		shutdownCalled = true;
	}


}