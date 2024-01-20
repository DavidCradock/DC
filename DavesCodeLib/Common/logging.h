#pragma once
#include "string.h"
#include <list>
#include <functional>	// For the function pointer using std::function

namespace DC
{
	// Logging class to help debug errors on people's machines aswell as my own.
	// The log entries are written to a text file.
	// The log entries are output to std::out, the console if the console exists (Like in debug mode)
	class Logging
	{
	public:
		// Enum to specify the status type of an entry
		enum EntryStatus
		{
			FAIL,		// [FAIL] entry
			INFO,		// [INFO] entry
			NOSTAT,		//        entry
			PASS,		// [PASS] entry
			WARN,		// [WARN] entry
			SEPERATOR	// Seperator text which 80 "-" characters
		};

		// Constructor to create the logging object
		// logFilenameParam is the name of the file where the log entries are saved to.
		Logging(const String& logFilenameParam = L"log.txt");

		// Adds a new log entry.
		// If the debug console is available, [FAIL] is output with the red colour before the log entry text
		// If addNewlineToEnd is true, will add "\n" to the end of the string before outputting.
		void addEntryFAIL(const String& logEntryText, bool addNewlineToEnd = true);

		// Adds a new log entry.
		// If the debug console is available, [INFO] is output with the default colour before the log entry text
		// If addNewlineToEnd is true, will add "\n" to the end of the string before outputting.
		void addEntryINFO(const String& logEntryText, bool addNewlineToEnd = true);

		// Adds a new log entry.
		// If the debug console is available, "    " is output with the default colour before the log entry text
		// If addNewlineToEnd is true, will add "\n" to the end of the string before outputting.
		void addEntryNOSTAT(const String& logEntryText, bool addNewlineToEnd = true);

		// Adds a new log entry.
		// If the debug console is available, [PASS] is output with the green colour before the log entry text
		// If addNewlineToEnd is true, will add "\n" to the end of the string before outputting.
		void addEntryPASS(const String& logEntryText, bool addNewlineToEnd = true);

		// Adds a new log entry.
		// If the debug console is available, [WARN] is output with the yellow colour before the log entry text
		// If addNewlineToEnd is true, will add "\n" to the end of the string before outputting.
		void addEntryWARN(const String& logEntryText, bool addNewlineToEnd = true);

		// Adds a new log entry.
		// This is simply 80 "-" characters with a newline at the end
		void addEntrySeperator(void);

		// Appends the ANSI escape code to the given String which sets the console to render the characters afterwards, red.
		void appendConsoleColourForRed(String& string);

		// Appends the ANSI escape code to the given String which sets the console to render the characters afterwards, green.
		void appendConsoleColourForGreen(String& string);

		// Appends the ANSI escape code to the given String which sets the console to render the characters afterwards, yellow.
		void appendConsoleColourForYellow(String& string);

		// Appends the ANSI escape code to the given String which sets the console to render the characters afterwards, the default colour of the console/terminal.
		void appendConsoleColourForDefault(String& string);

		// Sets a function to be called whenever an addEntry??? method is called.
		// The function should have void as it's return and a const String& containing the entry's text and a Logging::EntryStatus enumeration which specifies the type of entry added.
		// Example usage...
		// // Create a function to be called...
		// void someTest(const DC::String& string, DC::Logging::EntryStatus status)
		// {
		//	// Do stuff with the parsed data
		// }
		// Then somewhere in our code call setFunctionToCallOnAddEntry(someTest);
		// I have decided to add this, as I wish to have the user interface's console be able to recieve these debug entrys, but I do not wish to
		// modify the user interface's code directly within this class, instead, the function pointer will be set elsewhere within the code and dealt with there instead.
		// This prevents this class which is used everywhere from being coupled to the user interface which will make things easier if I later on, decide to remove/re-write the user interface code.
		void setFunctionToCallOnAddEntry(std::function<void(const String&, EntryStatus) > functionPointer)
		{
			this->functionPointer = functionPointer;
		}
	private:
		// Holds the name given to the constructor which holds the log filename which the log entries are written to.
		String logFilename;

		// Adds the given text to the logging file.
		void _addEntryToFile(const String& logEntryText);

		// Adds the given text to the debug console
		void _addEntryToConsole(const String& logEntryText);

		// Function pointer which can be set with setFunctionToCallOnAddedEntry()
		// It is called from within the various addEntrySTATUS() methods.
		std::function<void(const String&, EntryStatus)> functionPointer;
	};

	// Globally accessible log object of Logging class used throughout all code.
	extern Logging log;
}