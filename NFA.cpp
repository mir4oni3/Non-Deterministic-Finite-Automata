#include "NFA.h"
#include "RegexParser.h"
#include "Constants.h"
#include "Utils.h"
#include <cstring>
#include <iostream>

void NFA::resizeStates() {
	State* newStates = new State[2 * stateCapacity];
	for (size_t i = 0; i < stateCapacity; i++) {
		newStates[i] = states[i];
	}
	delete[] states;
	states = newStates;
	stateCapacity *= 2;
}

void NFA::resizeTransitions() {
	Transition* newTransitions = new Transition[2 * transitionCapacity];
	for (size_t i = 0; i < transitionCapacity; i++) {
		newTransitions[i] = transitions[i];
	}
	delete[] transitions;
	transitions = newTransitions;
	transitionCapacity *= 2;
}

void NFA::copyFrom(const NFA& other) {
	states = new State[other.stateCapacity];
	stateCount = other.stateCount;
	stateCapacity = other.stateCapacity;

	transitions = new Transition[other.transitionCapacity];
	transitionCount = other.transitionCount;
	transitionCapacity = other.transitionCapacity;

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
	stateCapacity = other.stateCapacity;

	transitions = other.transitions;
	other.transitions = nullptr;
	transitionCount = other.transitionCount;
	transitionCapacity = other.transitionCapacity;

	this->regex = std::move(other.regex);
	this->isDFA = other.isDFA;
}

void NFA::free() {
	delete[] states;
	delete[] transitions;
	transitions = nullptr;
	states = nullptr;
}

NFA::NFA() {
	states = new State[minStateCapacity];
	transitions = new Transition[minTransitionCapacity];
}

NFA::NFA(const MyString& regex) : NFA(std::move(RegexParser::NFAfromRegex(regex))){}

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
	addState(); // errorState
	size_t errorStateName = states[stateCount - 1].getName();

	for (size_t i = 0; i < stateCount; i++) {
		for (size_t j = 0; j < sizeof(alphabet); j++) {
			if (!transitionExists(states[i].getName(), alphabet[j], *this)) {
				addTransition(Transition(states[i], states[stateCount - 1], alphabet[j])); // add transition to errorState
			}
		}
	}
}

void NFA::addState(const State& other) { 
	if (stateCount == stateCapacity) {
		resizeStates();
	}
	states[stateCount] = other;
	//avoiding state name collision
	if (stateCount > 0 && states[stateCount].getName() <= states[stateCount - 1].getName()) {
		states[stateCount].setName(states[stateCount - 1].getName() + 1);
	}
	stateCount++;
}

void NFA::addState() {
	if (stateCount == stateCapacity) {
		resizeStates();
	}
	//avoiding state name collision
	if (stateCount == 0) {
		states[stateCount] = State(false, false, 0);
	}
	else {
		states[stateCount] = State(false, false, states[stateCount - 1].getName() + 1);
	}
	stateCount++;
}

void NFA::addTransition(const Transition& other) {
	if (transitionCount == transitionCapacity) {
		resizeTransitions();
	}
	transitions[transitionCount] = other;
	transitionCount++;
}

void NFA::print() const {
	std::cout << "States:" << std::endl;
	for (size_t i = 0; i < stateCount; i++) {
		states[i].print();
	}
	std::cout << "Transitions:" << std::endl;
	for (size_t i = 0; i < transitionCount; i++) {
		transitions[i].print();
	}
	std::cout << "Regex: " << regex << std::endl;
	std::cout << "This NFA is" << (isDFA ? "" : " not") << " a DFA";
}

bool NFA::isEmptyLanguage() const {
	return true;
}

bool NFA::accept(const MyString& word) const {
	MyVector<size_t> indices; // keeps the indices of all current states(parallel simulation)
	for (size_t i = 0; i < stateCount; i++) {
		if (states[i].isInitial()) {
			if (word.length() == 1 && word[0] == EPSILON && states[i].isFinal()) { // check for epsilon separately
				return true;
			}
			indices.push_back(i);
		}
	}
	addEpsilonTransitionStates(indices, *this);
	//indices is now filled with all starting states(and those that are reachable from the starting states with epsilon transitions)

	//performs epsilon transition for every letter in the word and keeps the current states in indices after each step
	for (size_t i = 0; i < word.length(); i++) {
		if (indices.getSize() == 0) {
			return false;
		}
		deltaTransition(indices, word[i], *this);
	}

	//if after reading the whole word, there is a final state in indices, that means the nfa accepts the word
	for (size_t i = 0; i < indices.getSize(); i++) {
		if (states[indices[i]].isFinal()) {
			return true;
		}
	}
	return false;
}

void Union(NFA& lhs, const NFA& rhs) { 
	addElementsFrom(lhs, rhs);
}

void Concat(NFA& lhs, const NFA& rhs) {
	//add new intermediate state between the two nfa's
	lhs.addState();
	unsigned intermediateStateIndex = lhs.stateCount - 1;

	MyString epsilonString = "E";
	epsilonString[0] = EPSILON;
	bool rhsContainsEpsilon = rhs.accept(epsilonString);

	//connect all finals from first nfa to the added state with epsilon transitions
	for (size_t i = 0; i < lhs.stateCount; i++) {
		if (lhs.states[i].isFinal()) {
			//if epsilon is not in the language of rhs, make final states of lhs non-final
			if (!rhsContainsEpsilon) {
				lhs.states[i] = State(lhs.states[i].isInitial(), false, lhs.states[i].getName());
			}
			lhs.addTransition(Transition(lhs.states[i],lhs.states[lhs.stateCount - 1], EPSILON));
		}
	}

	addElementsFrom(lhs, rhs);

	//connect intermediate state to all initial states from rhs with E-transitions(rhs states start with intermediateStateIndex + 1)
	for (size_t i = intermediateStateIndex + 1; i < lhs.stateCount; i++) {
		if (lhs.states[i].isInitial()) {
			lhs.addTransition(Transition(lhs.states[intermediateStateIndex], lhs.states[i], EPSILON));
			lhs.states[i] = State(false, lhs.states[i].isFinal(), lhs.states[i].getName()); //make non-initial
		}
	}
}

void Star(NFA& nfa) {
	//add new state which is initial and final so that epsilon is added
	nfa.addState(State(true, true, nfa.states[nfa.stateCount - 1].getName() + 1));

	for (size_t i = 0; i < nfa.stateCount; i++) {
		if (!nfa.states[i].isInitial()) {
			continue;
		}
		for (size_t j = 0; j < nfa.stateCount; j++) {
			if (nfa.states[j].isFinal()) {
				nfa.addTransition(Transition(nfa.states[j], nfa.states[i], EPSILON));
			}
		}
	}
}