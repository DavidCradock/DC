#include "finiteStateMachine.h"

namespace DC
{
	FiniteStateMachine::~FiniteStateMachine()
	{
		removeAllStates();
	}

	void FiniteStateMachine::update(void)
	{
		auto it = states.find(currentStateName);
		if (it != states.end())
			it->second->onActive(this);
	}

	void FiniteStateMachine::switchToState(const String& stateName)
	{
		auto it = states.find(stateName);
		ErrorIfTrue(it == states.end(), L"FiniteStateMachine::switchToState(\"" + stateName + L"\") failed. State name doesn't exist.");

		// Find currently set state
		std::map<String, FiniteStateBase*>::iterator itCurrentState = states.find(currentStateName);

		if (it == itCurrentState)
			return;

		// currentStateName might not be set, aka empty
		if (itCurrentState != states.end())
			itCurrentState->second->onExit();

		it->second->onEnter();

		currentStateName = stateName;
	}

	void FiniteStateMachine::addState(const String& stateName, FiniteStateBase* state)
	{
		auto it = states.find(stateName);
		ErrorIfTrue(it != states.end(), L"FiniteStateMachine::addState(\"" + stateName + L"\") failed. State name already exists.");
		ErrorIfFalse(state, L"FiniteStateMachine::addState(\"" + stateName + L"\") failed. The given state pointer was 0.");
		states[stateName] = state;
	}

	void FiniteStateMachine::removeState(const String& stateName)
	{
		auto it = states.find(stateName);
		ErrorIfTrue(it == states.end(), L"FiniteStateMachine::removeState(\"" + stateName + L"\") failed. State name doesn't exist.");

		if (currentStateName == it->first)
		{
			it->second->onExit();
			currentStateName.clear();
		}
		delete it->second;
		states.erase(it);
	}

	bool FiniteStateMachine::getStateExists(const String& stateName)
	{
		auto it = states.find(stateName);
		return (it != states.end());
	}

	void FiniteStateMachine::removeAllStates(void)
	{
		auto it = states.begin();
		while (it != states.end())
		{
			// If this is the currently set state, call it's onExit method
			if (currentStateName == it->first)
			{
				it->second->onExit();
				currentStateName.clear();
			}

			delete it->second;
			states.erase(it);
			it = states.begin();
		}
		states.clear();
	}

	size_t FiniteStateMachine::getNumberStates(void)
	{
		return states.size();
	}

	FiniteStateBase* FiniteStateMachine::getState(unsigned int index)
	{
		ErrorIfTrue(index >= (unsigned int)states.size(), L"FiniteStateMachine::getState() failed. Given invalid index.");
		auto it = states.begin();
		unsigned int uiCount = 0;
		while (uiCount < index)
		{
			uiCount++;
			it++;
		}
		return it->second;
	}

	FiniteStateBase* FiniteStateMachine::getState(const String& stateName)
	{
		auto it = states.find(stateName);
		ErrorIfTrue(it == states.end(), L"FiniteStateMachine::getState() failed. " + stateName + L" couldn't be found.");
		return it->second;
	}
}