#include "Transition.h"
#include <iostream>

const State& Transition::getInitialState() {
	return initialState;
}

const State& Transition::getResultState() {
	return resultState;
}

char Transition::getTransitionValue() {
	return transitionValue;
}

void Transition::setInitialStateName(unsigned name) {
	initialState.setName(name);
}

void Transition::setResultStateName(unsigned name) {
	resultState.setName(name);
}

void Transition::setTransitionValue(char value) {
	transitionValue = value;
}

Transition::Transition(const State& initial, const State& result, const char letter) : initialState(initial), resultState(result), transitionValue(letter) {}

Transition::Transition(const size_t initialName, const size_t resultName, const char letter) : initialState(0,0,initialName), resultState(0,0,resultName), transitionValue(letter){}

void Transition::print() const {
	std::cout << "(" << initialState.getName() << ", " << resultState.getName() << ", " << transitionValue << ")" << std::endl;
}