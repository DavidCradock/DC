#pragma once
#include "string.h"
#include <list>

namespace DC
{
	// Logging class to help debug errors on people's machines aswell as my own.
	// The log entries are written to a text file.
	// The log entries are output to std::out, the console if the console exists (Like in debug mode)
	class Logging
	{
	public:
		// Constructor to create the logging object
		// logFilenameParam is the name of the file where the log entries are saved to.
		Logging(const String& logFilenameParam = L"log.txt");

		// Adds a new log entry.
		// If the debug console is available, [INFO] is output with the default colour before the log entry text
		// If addNewlineToEnd is true, will add "\n" to the end of the string before outputting.
		void addEntryINFO(const String& logEntryText, bool addNewlineToEnd = true);

		// Adds a new log entry.
		// If the debug console is available, [    ] is output with the default colour before the log entry text
		// If addNewlineToEnd is true, will add "\n" to the end of the string before outputting.
		void addEntryNOSTAT(const String& logEntryText, bool addNewlineToEnd = true);

		// Adds a new log entry.
		// If the debug console is available, [PASS] is output with the green colour before the log entry text
		// If addNewlineToEnd is true, will add "\n" to the end of the string before outputting.
		void addEntryPASS(const String& logEntryText, bool addNewlineToEnd = true);

		// Adds a new log entry.
		// If the debug console is available, [FAIL] is output with the red colour before the log entry text
		// If addNewlineToEnd is true, will add "\n" to the end of the string before outputting.
		void addEntryFAIL(const String& logEntryText, bool addNewlineToEnd = true);

		// Adds a new log entry.
		// If the debug console is available, [WARN] is output with the yellow colour before the log entry text
		// If addNewlineToEnd is true, will add "\n" to the end of the string before outputting.
		void addEntryWARN(const String& logEntryText, bool addNewlineToEnd = true);

		// Adds a new log entry.
		// This is simply 80 "-" characters with a newline at the end
		void addEntrySeperator(void);

		// Appends the ANSI escape code to the given String which sets the console to render the characters afterwards, red.
		void appendConsoleColourForRed(String &string);

		// Appends the ANSI escape code to the given String which sets the console to render the characters afterwards, green.
		void appendConsoleColourForGreen(String& string);

		// Appends the ANSI escape code to the given String which sets the console to render the characters afterwards, yellow.
		void appendConsoleColourForYellow(String& string);

		// Appends the ANSI escape code to the given String which sets the console to render the characters afterwards, the default colour of the console/terminal.
		void appendConsoleColourForDefault(String& string);
	private:
		// Holds the name given to the constructor which holds the log filename which the log entries are written to.
		String logFilename;

		// Adds the given text to the logging file.
		void _addEntryToFile(const String& logEntryText);

		// Adds the given text to the debug console
		void _addEntryToConsole(const String& logEntryText);
	};

	// Globally accessible log object of Logging class used throughout all code.
	extern Logging log;
}