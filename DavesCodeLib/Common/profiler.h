#pragma once
#include "string.h"
#include "timerMinimal.h"
#include <map>

namespace DC
{
	// Used by Profiler class to return a vector of this struct containing the profiler's results
	struct ProfilerResults
	{
		// The section's name
		String sectionName;

		// The total accumulated time in seconds
		double accumulatedTimeSeconds;

		// The percentage of the "main" section that this section takes up.
		float percentageOfMain;
	};

	// A class to deal with time profiling.
	// It works on the concept of code sections.
	// We create an object of this class and then call
	// begin(L"nameOfSection") before any code we wish to time.
	// Then after all the section code, we call
	// end(L"nameOfSection")
	// If elsewhere in the code, we wish to time a section and add that time to the named section we've already used,
	// we add another begin()/end() pair and the time taken between those will be added to that code sections total time.
	// For everything to be computed properly, we MUST have a section called "main" which encapsulates all other sections.
	// This will allow the class to compute percentages each section takes.
	// Example:
	// Profiler prof;
	// void mainLoop(void)
	// {
	//   prof.begin(L"main");
	//   prof.begin(L"audio");
	//	 // Audio code goes here
	//	 prof.end(L"audio");
	//   prof.begin(L"gfx");
	//	 // Rendering code goes here
	//	 prof.end(L"gfx");
	//   prof.end(L"main");
	// }
	class Profiler
	{
	public:
		Profiler();

		// Begin timing a named section of code
		// There must be at least one section called "main" for things to work correctly.
		void begin(const String& sectionName = String(L"main"));

		// End timing a named section of code
		void end(const String& sectionName = String(L"main"));

		// Resets the profiler, removing all previously used sections, except for "main"
		void reset(void);

		// Returns the number of accumulated seconds for the named section.
		// If the section doesn't exist, an exception occurs.
		double getSectionTime(const String& sectionName = String(L"main"));

		// Returns the total number of sections including "main"
		size_t getSectionNumber(void);

		// Returns a vector of ProfilerResults, sorted by accumulated time in seconds.
		// Unfortunately, this only returns valid results after a call to end() with the "main" section specified has been called,
		// otherwise we don't have all the needed information we need to compute the percentages of main.
		// However, passing true to returnCachedResults will return the results from results which have been obtained during the
		// last call to end() with "main" being given as the section. These results are one frame old, but are complete.
		std::vector<ProfilerResults> getResults(bool returnCachedResults = true);

		// Members needed by each code section.
		struct SSection
		{
			// Minimal timer object used to time this section
			TimerMinimal timer;

			// Total accumulated time since call or calls between begin and end
			double accumulatedTimeInSeconds;

			// Number of times begin() has been called for this named section since
			// begin() was called on "main" section.
			int beginCalledCount;
		};
	private:
		// Holds each named section including "main"
		std::map<String, SSection> sections;

		// Count until update _mstrResultsText.
		float updateResultsText;

		// Holds results text to be printed to the screen.
		std::vector<String> resultsText;

		TimerMinimal timerMinimal;

		// This holds the cached results which are created upon a call to end() with "main" being the named section
		std::vector<ProfilerResults> resultsCached;
	};
}