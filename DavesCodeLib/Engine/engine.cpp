#include "engine.h"
#include "../Common/error.h"
#include "../Common/logging.h"



namespace DC
{
	Engine::Engine()
	{
		application = 0;
		shutdownCalled = false;
	}

	void Engine::go(ApplicationBase* pApplication)
	{
		log.addEntrySeperator();
		log.addEntryINFO(L"Engine::go() called.");
		log.addEntrySeperator();

		ErrorIfTrue(application, L"Engine::go() failed. go already called as existing application pointer is not 0.");
		application = pApplication;

		renderer.init(settings);

		application->onInit();
		
		log.addEntrySeperator();
		log.addEntryINFO(L"Entering main loop.");
		log.addEntrySeperator();
		while (!shutdownCalled)
		{
			timer.update();
			application->onUpdate(timer.getSecondsPast(), this);
			if (!renderer.update())
				shutdownCalled = true;
		}

		log.addEntrySeperator();
		log.addEntryINFO(L"Engine::go() main application loop has ended.");
		log.addEntrySeperator();

		application->onShutdown();

		renderer.shutdown();
	}

	void Engine::shutdown(void)
	{
		log.addEntryINFO(L"Engine::shutdown() called.");
		shutdownCalled = true;
	}


}