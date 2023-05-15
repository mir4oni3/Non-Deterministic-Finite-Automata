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
	Transition();
	Transition(const State& initial, const State& result, const char letter);
};