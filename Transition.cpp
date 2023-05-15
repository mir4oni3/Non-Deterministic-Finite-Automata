#include "Transition.h"

const State& Transition::getInitialState() {
	return initialState;
}

const State& Transition::getResultState() {
	return resultState;
}

char Transition::getTransitionValue() {
	return transitionValue;
}

Transition::Transition() {
	transitionValue = '\0';
}

Transition::Transition(const State& initial, const State& result, const char letter) {
	initialState = initial;
	resultState = result;
	transitionValue = letter;
}