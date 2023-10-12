#pragma once

namespace DC
{
	// Joystick input class, use InputManager to access it's "joy" member.
	// There is an object of this class called "joy" which is initialised and updated in the InputManager class.
	// Use that object to access information about the device. 
	class InputJoystick
	{
	public:
		InputJoystick(void);
		~InputJoystick(void);

	private:

	};

}