#include "settings.h"
#include "../Common/error.h"
#include "../Common/utilities.h"
#include "../Common/logging.h"

namespace DC
{
	Settings::Settings()
	{
		// Attempt to load from file, if fail, set defaults.
		load();
	}

	bool Settings::load(const String& strFilename)
	{
		String logEntry(L"Settings::load(\"");
		logEntry.append(strFilename);
		logEntry.append(L"\") called.");
		log.addEntryINFO(logEntry);

		// Open file in text mode
		std::ifstream file;
		file.open(strFilename, std::ifstream::in);
		if (!file.is_open())
		{
			log.addEntryWARN(L"Settings::load() unable to load settings file.");
			_setAllToDefault();
			return false;
		}

		// Filename
		std::string strWord;
		std::string strLine;
		char space;
		getline(file, strLine);

		// backbufferScale
		file >> strWord;	space = file.get();	// Setting name, then space
		//getline(file, strLine);
		file >> backbufferScale;
		
		// windowFullscreen
		file >> strWord;	space = file.get();	// Setting name, then space
		file >> windowFullscreen;
		
		// windowVSync
		file >> strWord;	space = file.get();	// Setting name, then space
		file >> windowVSync;
		
		// windowWidthWindowed
		file >> strWord;	space = file.get();	// Setting name, then space
		file >> windowWidthWindowed;
		
		// windowHeightWindowed
		file >> strWord;	space = file.get();	// Setting name, then space
		file >> windowHeightWindowed;
		
		// UIVolume
		file >> strWord;	space = file.get();	// Setting name, then space
		file >> UIVolume;

		// UITooltipDelaySeconds
		file >> strWord;	space = file.get();	// Setting name, then space
		file >> UITooltipDelaySeconds;
		
		// applicationName
		file >> strWord;	space = file.get();	// Setting name, then space
		getline(file, strLine);
		applicationName.clear();
		applicationName.appendString(strLine);

		// useVulkanValidationLayers
		file >> strWord;	space = file.get();	// Setting name, then space
		file >> useVulkanValidationLayers;

		// If an error occurred
		if (file.fail())
		{
			log.addEntryWARN(L"Failure occurred whilst reading in settings from file.");
			_setAllToDefault();
			file.close();
			save();
			return false;
		}

		log.addEntryPASS(L"Settings loading complete.");
		_logCurrentSettings();
		log.addEntrySeperator();
		file.close();
		return true;
	}

	void Settings::save(const String& strFilename)
	{
		log.addEntryINFO(L"Settings::save() called.");

		// Open file in text mode, deleting contents if existed before
		std::ofstream file;
		file.open(strFilename, std::ofstream::out | std::ofstream::trunc);
		if (!file.is_open())
		{
			String err;
			err += L"Settings::saveToFile(";
			err += strFilename;
			err += L") failed to open file for writing.";
			ErrorIfFalse(0, err);
		}

		String str;

		str = strFilename;
		str += L"\n";
		file << str.c_strn();

		// backbufferScale
		str = L"BackbufferScale: ";
		str.appendFloat(backbufferScale);
		str += L"\n";
		file << str.c_strn();

		// windowFullscreen
		str = L"WindowFullscreen: ";
		str.appendInt(int(windowFullscreen));
		str += L"\n";
		file << str.c_strn();

		// windowVSync
		str = L"WindowVSync: ";
		str.appendInt(int(windowVSync));
		str += L"\n";
		file << str.c_strn();

		// windowWidthWindowed
		str = L"WindowWidthWindowed: ";
		str.appendInt(windowWidthWindowed);
		str += L"\n";
		file << str.c_strn();

		// windowHeightWindowed
		str = L"WindowHeightWindowed: ";
		str.appendInt(windowHeightWindowed);
		str += L"\n";
		file << str.c_strn();

		// UIVolume
		str = L"UIVolume: ";
		str.appendFloat(UIVolume, 2);
		str += L"\n";
		file << str.c_strn();

		// UITooltipDelaySeconds
		str = L"UITooltipDelaySeconds: ";
		str.appendFloat(UITooltipDelaySeconds, 2);
		str += L"\n";
		file << str.c_strn();

		// applicationName
		str = L"ApplicationName: ";
		str.appendString(applicationName.c_strn());
		str += L"\n";
		file << str.c_strn();

		// useVulkanValidationLayers
		str = L"useVulkanValidationLayers: ";
		str.appendInt(int(useVulkanValidationLayers));
		str += L"\n";
		file << str.c_strn();

		// Make sure there were no errors
		if (file.fail())
		{
			String err = L"Settings::save(";
			err += strFilename;
			err += L") failed whilst saving file.";
			ErrorIfTrue(1, err);
		}
		file.close();

		_logCurrentSettings();
	}

	void Settings::_setAllToDefault(void)
	{
		log.addEntryINFO(L"Settings::_setAllToDefault() called.");

		backbufferScale = 1.0f;
		windowFullscreen = false;
		windowVSync = true;
		windowWidthWindowed = 1920;
		windowHeightWindowed = 1080;
		UIVolume = 1.0f;
		UITooltipDelaySeconds = 1.0f;
		applicationName = L"DC Development Application.";
		useVulkanValidationLayers = true;

		_logCurrentSettings();
	}

	void Settings::setBackbufferScale(float fBackbufferScale)
	{
		clamp(fBackbufferScale, 0.01f, 4.0f);
		backbufferScale = fBackbufferScale;
		save();
	}

	float Settings::getBackbufferScale(void) const
	{
		return backbufferScale;
	}

	bool Settings::getWindowFullscreen(void) const
	{
		return windowFullscreen;
	}

	void Settings::setWindowFullscreen(bool bFullscreen)
	{
		windowFullscreen = bFullscreen;
		save();
	}

	bool Settings::getWindowVSync(void) const
	{
		return windowVSync;
	}

	void Settings::setWindowVSync(bool bFullscreen)
	{
		windowVSync = bFullscreen;
		save();
	}

	int Settings::getWindowWidthWhenWindowed(void) const
	{
		return windowWidthWindowed;
	}

	void Settings::setWindowWidthWhenWindowed(int windowWidthIn)
	{
		windowWidthWindowed = windowWidthIn;
		save();
	}

	int Settings::getWindowHeightWhenWindowed(void) const
	{
		return windowHeightWindowed;
	}

	void Settings::setWindowHeightWhenWindowed(int windowHeightIn)
	{
		windowHeightWindowed = windowHeightIn;
		save();
	}

	void Settings::setUIVolume(float fVolume)
	{
		UIVolume = fVolume;
		clamp(UIVolume, 0.0f, 1.0f);
		save();
	}

	float Settings::getUIVolume(void) const
	{
		return UIVolume;
	}

	void Settings::setUITooltipDelay(float fDelayInSeconds)
	{
		UITooltipDelaySeconds = fDelayInSeconds;
		save();
	}

	float Settings::getUITooltipDelaySeconds(void) const
	{
		return UITooltipDelaySeconds;
	}

	void Settings::setApplicationName(const String& applicationNamePARAM)
	{
		applicationName = applicationNamePARAM;
	}

	String Settings::getApplicationName(void) const
	{
		return applicationName;
	}

	void Settings::setVulkanValidationLayersUsage(bool useValidationLayers)
	{
		useVulkanValidationLayers = useValidationLayers;
	}

	bool Settings::getVulkanValidationLayersUsage(void) const
	{
		return useVulkanValidationLayers;
	}

	void Settings::_logCurrentSettings(void)
	{
		log.addEntryINFO(L"Listing settings...");
		String logEntry;
		logEntry = L"backbufferScale: ";
		logEntry.appendFloat(backbufferScale);
		log.addEntryNOSTAT(logEntry);

		logEntry = L"windowFullscreen: ";
		logEntry.appendInt((int)windowFullscreen);
		log.addEntryNOSTAT(logEntry);

		logEntry = L"windowVSync: ";
		logEntry.appendInt((int)windowVSync);
		log.addEntryNOSTAT(logEntry);

		logEntry = L"windowWidthWindowed: ";
		logEntry.appendInt(windowWidthWindowed);
		log.addEntryNOSTAT(logEntry);

		logEntry = L"windowHeightWindowed: ";
		logEntry.appendInt(windowHeightWindowed);
		log.addEntryNOSTAT(logEntry);

		logEntry = L"UIVolume: ";
		logEntry.appendFloat(UIVolume);
		log.addEntryNOSTAT(logEntry);

		logEntry = L"UITooltipDelaySeconds: ";
		logEntry.appendFloat(UITooltipDelaySeconds);
		log.addEntryNOSTAT(logEntry);

		logEntry = L"applicationName: ";
		logEntry.append(applicationName);
		log.addEntryNOSTAT(logEntry);

		logEntry = L"useVulkanValidationLayers: ";
		logEntry.appendInt((int)useVulkanValidationLayers);
		log.addEntryNOSTAT(logEntry);
	}
}