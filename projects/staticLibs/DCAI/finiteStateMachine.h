#pragma once
#include "../DCCommon/DCCommon.h"
#include <map>

namespace DC
{
	class FiniteStateMachine;

	// Finite state base class with which to derive our FSM state classes from for use with the FiniteStateMachine class.
	// This represents a single state in a finite state machine and has several pure virtual methods which need to be 
	// implemented within any class which is derived from this one.
	class FiniteStateBase
	{
	public:
		// This is called when this state is first entered.
		virtual void onEnter(void) = 0;

		// This is called when this state is left.
		virtual void onExit(void) = 0;

		// This is called whilst the state is active.
		// pFSM is a pointer to the FiniteStateMachine this state is stored in.
		// This is usefull to allow the state to change to another named state by calling pFSM->switchToState()
		virtual void onActive(FiniteStateMachine* pFSM) = 0;
	};

	// This is a generic finite state machine used alongside classes derived from FiniteStateBase.
	// Each state has three methods which need to be implemented (See FiniteStateBase)
	// First create one or more classes derived from the base class and implement each of the three methods,
	// onEnter(), onExit() and onActive().
	// We create an object of this class and objects of each of our classes derived from the FiniteStateBase class.
	// Then we add each of those objects to this one, specifying a unique name for each state.
	// Initially, the state machine has no state and upon calling update() will do nothing.
	// We need to call switchToState() to set the initial state.
	// Upon doing so, the state's onEnter() method will be called, followed by it's onActive() method, until the
	// state machine is asked to switch to another state with a call of switchToState() either externally, or from
	// within a state itself.
	// Usage example:
	// First, create a couple of classes derived from the FiniteStateBase class which will be used for the FSM's states
	// class MyFirstState : public FiniteStateBase
	// {
	// public:
	//		void onEnter(void){	// Do stuff for when this state is entered	}
	//		void onExit(void){	// Do stuff for when this state is left	}
	//		void onActive(FiniteStateMachine* pFSM){ pFSM->switchToState("MySecondState");	// Switch to the second state }
	// };
	// class MySecondState : public FiniteStateBase
	// {
	// public:
	//		void onEnter(void){	// Do stuff for when this state is entered	}
	//		void onExit(void){	// Do stuff for when this state is left	}
	//		void onActive(FiniteStateMachine* pFSM){ // Do stuff in this state }
	// };
	// // Now that the two state classes have been created, we need to create the FSM itself and allocate memory for each of the state classes.
	// FiniteStateMachine fsm;
	// MyFirstState* pStateOne = new MyFirstState;
	// MySecondState* pStateTwo = new MySecondState;
	// // Now we need to add each state to the FSM, giving them each a unique name...
	// fsm.addState("state1", pStateOne);
	// fsm.addState("state2", pStateTwo);
	// // Note: There's no need to delete either of the states, as this is done for us
	// // when the FSM is destroyed, or by calling either removeState() or removeAllStates() of the FSM object.
	// // Now call fsm.update() to call the currently set state's onActive() method.
	class FiniteStateMachine
	{
	public:
		// Frees the state pointers which have been added with a call to addState()
		~FiniteStateMachine();

		// Updates the state machine, calling the currently set state's onActive() method.
		// Remember that initially, no current state is set, so this will initially do nothing until switchToState() is called.
		void update(void);

		// Switches to the named state.
		// This usually calls the currently set state's onExit() method, followed by the new state's onEnter() method.
		// But the first time this is called, the FSM has no currently set state, so the call to onExit() is not performed.
		// If the named state doesn't exist, then an exception occurs.
		void switchToState(const String& stateName);

		// Adds a new uniquely named state
		// If the name already exists, an exception occurs.
		// The FiniteStateBase object has to be allocated yourself prior to calling this method.
		// Once the state object has been added here, there's no need to keep it around for deletion later,
		// as this class's removeState(), removeAllStates() and destructor will delete them for us.
		// Also note that this method checks to see if the given state pointer is NULL and throws an exception if so.
		void addState(const String& stateName, FiniteStateBase* pState);

		// Removes a previously added state
		// If the named state doesn't exist, an exception occurs
		// The state given during a call to addState() is deleted for us here, no need to delete it ourselves.
		void removeState(const String& stateName);

		// Returns whether a named state has already been added or not
		bool getStateExists(const String& stateName);

		// Removes all previously added states
		// The states given during calls to addState() are deleted for us here, no need to delete them ourselves.
		void removeAllStates(void);

		// Returns the number of states
		size_t getNumberStates(void);

		// Returns pointer to the state at the given index.
		// If an invalid index is given, an exception occurs
		FiniteStateBase* getState(unsigned int index);

		// Returns pointer to the named state.
		// If the named state couldn't be found, an exception occurs.
		FiniteStateBase* getState(const String& stateName);
	private:
		String currentStateName;						// The name of the currently set state. This is initially empty, IE, there's no state.
		std::map<String, FiniteStateBase*> states;		// Holds each uniquely named state and a pointer to the object.
	};
}