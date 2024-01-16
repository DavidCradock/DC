#pragma once
#include "../Common/string.h"

namespace DC
{
	// Holds all engine settings.
	// These settings are things which are usually accessed from a program's settings interface and include such things as..
	// Shadow map resolution, backbuffer resolution, global audio volume and more.
	class Settings
	{
	public:

		// Default constructor which sets, settings to their defaults.
		Settings();

		// Attempts to load all the settings from a file.
		// If the file couldn't be found, all the settings are set to default values, the settings are saved and false is returned, else true
		bool load(const String& strFilename = L"settings.cfg");

		// Saves all settings to a file.
		void save(const String& strFilename = L"settings.cfg");

		// Sets the multiplier for the back buffer framebuffer which many things are rendered to.
		// This, by default is set to 1.0.
		// A value of 1.0f is 100 percent of the window dims.
		// Setting this smaller will reduce GPU usage.
		// Setting this greater will increase quality.
		// The given value is clamped between 0.1 - 4.0
		// This also resizes the framebuffer used by the back buffer.
		// This re-saves the settings file.
		void setBackbufferScale(float fBackbufferScale);

		// Returns the currently set back buffer scaling value.
		// See setBackbufferScale for more information.
		float getBackbufferScale(void) const;

		// Returns whether the application's window should be created full screen or not.
		bool getWindowFullscreen(void) const;

		// Sets whether the application's window should be created full screen or not.
		// This re-saves the settings file.
		void setWindowFullscreen(bool bFullscreen);

		// Returns whether the application's window should set VSync on or off.
		bool getWindowVSync(void) const;

		// Sets whether the application's window should set VSync on or off.
		// This re-saves the settings file.
		void setWindowVSync(bool bFullscreen);

		// Returns the width of the window used when running in windowed mode
		int getWindowWidthWhenWindowed(void) const;

		// Sets the width of the window used when running in windowed mode
		// This re-saves the settings file.
		void setWindowWidthWhenWindowed(int windowWidthIn);

		// Returns the height of the window used when running in windowed mode
		int getWindowHeightWhenWindowed(void) const;

		// Sets the height of the window used when running in windowed mode
		// This re-saves the settings file.
		void setWindowHeightWhenWindowed(int windowHeightIn);

		// Sets the global volume level for the user interface ranging from 0 to 1
		// This re-saves the settings file.
		void setUIVolume(float fVolume);

		// Returns the global volume level for the user interface ranging from 0 to 1
		float getUIVolume(void) const;

		// Sets the user interfaces tooltip delay in seconds.
		// This is the amount of time in seconds which the mouse cursor has to be over a widget,
		// before the widget's tooltip will start to appear.
		// This re-saves the settings file.
		void setUITooltipDelay(float fDelayInSeconds);

		// Returns the user interfaces tooltip delay in seconds.
		// This is the amount of time in seconds which the mouse cursor has to be over a widget,
		// before the widget's tooltip will start to appear.
		float getUITooltipDelaySeconds(void) const;

		// Sets the application name used when creating the application window
		void setApplicationName(const String& applicationNamePARAM);

		// Returns the application name used when creating the application window
		String getApplicationName(void);
	private:

		// Sets all settings to default values
		void _setAllToDefault(void);

		// Logs current settings
		void _logCurrentSettings(void);

		// Multiplies the dimensions of the application's window and is used to set the scale of the
		// backbuffer frame buffer which alot of things are rendered to.
		float backbufferScale;

		// Whether the application window should be created windowed or fullscreen
		bool windowFullscreen;

		// Whether the application window should set VSync to enabled or not
		bool windowVSync;

		// Width of the window when running in windowed mode
		int windowWidthWindowed;

		// Height of the window when running in windowed mode
		int windowHeightWindowed;

		// Global volume scale for the user interface
		float UIVolume;

		// The user interfaces tooltip delay in seconds.
		// This is the amount of time in seconds which the mouse cursor has to be over a widget,
		// before the widget's tooltip will start to appear.
		float UITooltipDelaySeconds;

		// The application name used when creating the application window
		String applicationName;
	};
}