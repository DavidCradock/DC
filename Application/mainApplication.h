#pragma once
#include "../DavesCodeLib/DavesCode.h"

class MainApplication : public DC::ApplicationBase
{
public:
	void onUpdate(double timeInSecondsSinceLastCall, DC::Engine* pEngine);

	void onInit(void);

	void onShutdown(void);
private:

};