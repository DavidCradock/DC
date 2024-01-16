#include "logging.h"
#include <iostream>

namespace DC
{
	// Globally accessible log object of Logging class used throughout all code.
	Logging log(L"log.txt");

	Logging::Logging(const String& logFilenameParam)
	{
		logFilename = logFilenameParam;

		// Open file in text mode, deleting contents if existed before.
		// Write out name of debug filename at the top and then close the file.
		std::ofstream file;
		file.open(logFilename, std::ofstream::out | std::ofstream::trunc);
		if (!file.is_open())
		{
		}
		String str;
		str = logFilenameParam;
		str += L"\n";
		file << str.c_strn();
		file.close();
	}
	
	void Logging::addEntryINFO(const String& logEntryText, bool addNewlineToEnd)
	{
		String output = logEntryText;
		if (addNewlineToEnd)
			output.append(L"\n");
		_addEntryToFile(output);

		output.clear();
		appendConsoleColourForDefault(output);
		output.append(L"[INFO] ");
		output.append(logEntryText);
		if (addNewlineToEnd)
			output.append(L"\n");
		_addEntryToConsole(output);
	}

	void Logging::addEntryNOSTAT(const String& logEntryText, bool addNewlineToEnd)
	{
		String output = logEntryText;
		if (addNewlineToEnd)
			output.append(L"\n");
		_addEntryToFile(output);

		output.clear();
		appendConsoleColourForDefault(output);
		output.append(L"[    ] ");
		output.append(logEntryText);
		if (addNewlineToEnd)
			output.append(L"\n");
		_addEntryToConsole(output);
	}

	void Logging::addEntryPASS(const String& logEntryText, bool addNewlineToEnd)
	{
		String output = logEntryText;
		if (addNewlineToEnd)
			output.append(L"\n");
		_addEntryToFile(output);

		output.clear();
		appendConsoleColourForDefault(output);
		output.append(L"[");
		appendConsoleColourForGreen(output);
		output.append(L"PASS");
		appendConsoleColourForDefault(output);
		output.append(L"] ");
		output.append(logEntryText);
		if (addNewlineToEnd)
			output.append(L"\n");
		_addEntryToConsole(output);
	}

	void Logging::addEntryFAIL(const String& logEntryText, bool addNewlineToEnd)
	{
		String output = logEntryText;
		if (addNewlineToEnd)
			output.append(L"\n");
		_addEntryToFile(output);

		output.clear();
		appendConsoleColourForDefault(output);
		output.append(L"[");
		appendConsoleColourForRed(output);
		output.append(L"FAIL");
		appendConsoleColourForDefault(output);
		output.append(L"] ");
		output.append(logEntryText);
		if (addNewlineToEnd)
			output.append(L"\n");
		_addEntryToConsole(output);
	}

	void Logging::addEntryWARN(const String& logEntryText, bool addNewlineToEnd)
	{
		String output = logEntryText;
		if (addNewlineToEnd)
			output.append(L"\n");
		_addEntryToFile(output);

		output.clear();
		appendConsoleColourForDefault(output);
		output.append(L"[");
		appendConsoleColourForYellow(output);
		output.append(L"WARN");
		appendConsoleColourForDefault(output);
		output.append(L"] ");
		output.append(logEntryText);
		if (addNewlineToEnd)
			output.append(L"\n");
		_addEntryToConsole(output);
	}

	void Logging::addEntrySeperator(void)
	{
		static const String output = L"--------------------------------------------------------------------------------\n";
		_addEntryToFile(output);
		_addEntryToConsole(output);
	}

	void Logging::appendConsoleColourForRed(String& string)
	{
		string.append(L"\033[31m");
	}
	
	void Logging::appendConsoleColourForGreen(String& string)
	{
		string.append(L"\033[32m");
	}

	void Logging::appendConsoleColourForYellow(String& string)
	{
		string.append(L"\033[33m");
	}

	void Logging::appendConsoleColourForDefault(String& string)
	{
		string.append(L"\033[0m");
	}

	void Logging::_addEntryToFile(const String& logEntryText)
	{
		// Open file in text mode, appending to the thing
		std::ofstream file;
		file.open(logFilename, std::ios_base::app);
		if (!file.is_open())
		{
			return;
		}
		file << logEntryText.c_strn();
		file.close();
	}

	void Logging::_addEntryToConsole(const String& logEntryText)
	{
		std::cout << logEntryText.c_strn();
	}
}