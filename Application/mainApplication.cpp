#include "mainApplication.h"

void MainApplication::onUpdate(double timeInSecondsSinceLastCall, DC::Engine* pEngine)
{
//	pEngine->shutdown();
}

void MainApplication::onInit(void)
{
	DC::log.addEntryINFO(L"MainApplication::onInit() called.");
}

void MainApplication::onShutdown(void)
{
	DC::log.addEntryINFO(L"MainApplication::onShutdown() called.");
}
