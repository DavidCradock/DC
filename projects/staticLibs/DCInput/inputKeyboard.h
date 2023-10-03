#pragma once
#include "../DCCommon/timerMinimal.h"

namespace DC
{


	// Keyboard Input class, use InputManager to access it's "key" member.
	// There is an object of this class called "key" which is initialised and updated in the InputManager class.
	// Use that object to access information about the device.
	class InputKeyboard
	{
	public:
		InputKeyboard(void);
		~InputKeyboard(void);


	private:

	};

}