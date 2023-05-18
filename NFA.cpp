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

void NFA::moveFrom(NFA&& other) {
	states = other.states;
	other.states = nullptr;
	stateCount = other.stateCount;

	transitions = other.transitions;
	other.transitions = nullptr;
	transitionCount = other.transitionCount;

	this->regex = std::move(other.regex);
	this->isDFA = other.isDFA;
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

NFA::NFA(NFA&& other) noexcept {
	moveFrom(std::move(other));
}

NFA& NFA::operator=(NFA&& other) noexcept {
	if (this != &other) {
		free();
		moveFrom(std::move(other));
	}
	return *this;
}

State& NFA::operator[](const size_t index) const {
	if (index < 0 || index >= stateCount) {
		throw std::out_of_range("invalid index operator[]");
	}
	return states[index];
}

void NFA::determinate() {

}

void NFA::minimise() {

}

void NFA::makeTotal() {

}

void NFA::addState(const State& other) {

}

void NFA::addTransition(const Transition& other) {

}

bool NFA::isEmptyLanguage() const {
	return true;
}

bool NFA::accept(const MyString& word) const {
	return true;
}

void Union(NFA& lhs, const NFA& rhs) {

}

void Concat(NFA& lhs, const NFA& rhs) {

}

void Star(NFA& nfa) {

}