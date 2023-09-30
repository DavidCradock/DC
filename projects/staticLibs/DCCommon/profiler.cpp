#include "profiler.h"
#include "error.h"
#include <algorithm>

namespace DC
{
	Profiler::Profiler()
	{
		reset();
	}

	void Profiler::reset(void)
	{
		sections.clear();
		updateResultsText = 0.0f;
	}

	void Profiler::begin(const String& sectionName)
	{
		// Add named section if it doesn't exist.
		auto it = sections.find(sectionName);
		if (sections.end() == it)
		{
			SSection newSection;
			newSection.accumulatedTimeInSeconds = 0;
			newSection.beginCalledCount = 0;
			sections[sectionName] = newSection;
			it = sections.find(sectionName);
		}

		// If this section is "main", then reset all sections
		if (L"main" == sectionName)
		{
			auto it2 = sections.begin();
			while (it2 != sections.end())
			{
				it2->second.accumulatedTimeInSeconds = 0;
				it2->second.beginCalledCount = 0;
				it2++;
			}
		}

		it->second.timer.update();
		it->second.beginCalledCount++;
		if (it->second.beginCalledCount == 1)
			it->second.accumulatedTimeInSeconds = 0;
	}

	void Profiler::end(const String& sectionName)
	{
		// If named section doesn't exist, throw an exception.
		auto it = sections.find(sectionName);
		ErrorIfTrue(sections.end() == it, L"Profiler::end(\"" + sectionName + L"\") failed. The named section doesn't exist.");

		// Update this section's accumulated time
		it->second.timer.update();
		it->second.accumulatedTimeInSeconds += it->second.timer.getSecondsPast();

		// If this section is "main", update the cached results.
		if (L"main" == sectionName)
		{
			resultsCached.clear();
			resultsCached = getResults(false);
		}
	}

	double Profiler::getSectionTime(const String& sectionName)
	{
		// If named section doesn't exist, throw an exception.
		auto it = sections.find(sectionName);
		ErrorIfTrue(sections.end() == it, L"Profiler::getSectionTime(\"" + sectionName + L"\") failed. The named section doesn't exist.");
		return it->second.accumulatedTimeInSeconds;
	}

	size_t Profiler::getSectionNumber(void)
	{
		return sections.size();
	}

	bool sortByVal(const std::pair<String, Profiler::SSection>& a, const std::pair<String, Profiler::SSection>& b)
	{
		return (a.second.accumulatedTimeInSeconds > b.second.accumulatedTimeInSeconds);
	}

	std::vector<ProfilerResults> Profiler::getResults(bool returnCachedResults)
	{
		if (returnCachedResults)
		{
			return resultsCached;
		}

		// Will hold the final results
		std::vector<ProfilerResults> vResults;

		// Create a vector of pairs
		std::vector<std::pair<String, Profiler::SSection>> vec;

		// Copy key-value pairs from the map to the vector
		std::map<String, Profiler::SSection> ::iterator it;
		for (it = sections.begin(); it != sections.end(); it++)
		{
			vec.push_back(make_pair(it->first, it->second));
		}

		// Sort the vector by increasing order of its pair's second value's dAccumulatedTimeInSeconds value.
		sort(vec.begin(), vec.end(), sortByVal);

		// Find main and get it's accumulated time
		auto itMain = sections.find(String(L"main"));
		ErrorIfTrue(sections.end() == itMain, L"Profiler::getResults() failed. The \"main\" section doesn't exist.");
		double dMainTime = itMain->second.accumulatedTimeInSeconds;

		// Prevent divide by zero error below.
		if (dMainTime <= 0)
			dMainTime = 0.00001;

		// Now copy vector to vector and return
		for (size_t i = 0; i < vec.size(); i++)
		{
			ProfilerResults results;
			results.accumulatedTimeSeconds = vec[i].second.accumulatedTimeInSeconds;
			results.percentageOfMain = float(results.accumulatedTimeSeconds / dMainTime);
			results.percentageOfMain *= 100;
			results.sectionName = vec[i].first;
			vResults.push_back(results);
		}
		return vResults;
	}
}