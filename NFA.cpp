#include "NFA.h"
#include "RegexParser.h"
#include "Constants.h"
#include <cstring>
#include <iostream>

void NFA::copyFrom(const NFA& other) {
	states = new State[other.stateCount];
	stateCount = other.stateCount;
	transitions = new Transition[other.transitionCount];
	transitionCount = other.transitionCount;

	this->regex = other.regex;
	this->isDFA = other.isDFA;
	for (int i = 0; i < stateCount; i++) {
		states[i] = other.states[i];
	}
	for (int i = 0; i < transitionCount; i++) {
		transitions[i] = other.transitions[i];
	}
}

void NFA::free() {
	delete[] states;
	delete[] transitions;
	transitions = nullptr;
	states = nullptr;
}

NFA::NFA(const MyString& regex) : NFA(std::move(RegexParser::NFAfromRegex(regex))) {}

NFA::~NFA() {
	free();
}

NFA::NFA(const NFA& other) {
	copyFrom(other);
}

NFA& NFA::operator=(const NFA& other) {
	if (this != &other) {
		free();
		copyFrom(other);
	}
	return *this;
}

NFA::NFA(NFA&& other) noexcept : regex(std::move(other.regex)), states(other.states), transitions(other.transitions), 
								 isDFA(other.isDFA), stateCount(other.stateCount), transitionCount(other.transitionCount){
	other.states = nullptr;
	other.transitions = nullptr;
}

NFA& NFA::operator=(NFA&& other) noexcept {
	if (this != &other) {
		free();

		regex = std::move(other.regex);
		states = other.states;
		transitions = other.transitions;
		isDFA = other.isDFA;
		stateCount = other.stateCount;
		transitionCount = other.transitionCount;

		other.states = nullptr;
		other.transitions = nullptr;
	}
	return *this;
}

State& NFA::operator[](const size_t index) const {
	if (index < 0 || index >= stateCount) {
		throw std::out_of_range("invalid index operator[]");
	}
	return states[index];
}
