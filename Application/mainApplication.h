#pragma once
#include "../DavesCodeLib/DavesCode.h"

class MainApplication : public DC::ApplicationBase
{
public:
	void onUpdate(double timeInSecondsSinceLastCall, DC::Framework* pFramework);

	void onInit(void);

	void onShutdown(void);
private:

};