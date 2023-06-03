#pragma once
#include "State.h"

class Transition {
	State initialState;
	State resultState;
	char transitionValue;
public:
	const State& getInitialState();
	const State& getResultState();
	char getTransitionValue();

	void setInitialStateName(unsigned name);
	void setResultStateName(unsigned name);
	void setTransitionValue(char value);

	Transition() = default;
	Transition(const State& initial, const State& result, const char letter);
	void print() const;
};