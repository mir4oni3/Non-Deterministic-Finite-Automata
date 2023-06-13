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
	if (isDFA) {
		return;
	}

	Utils::makeSingleInitialAndFinal(*this);//state[stateCount - 1] is final and state[stateCount - 2] is initial
	MyVector<MyVector<MyVector<size_t>>> table; // table[i][0] is the new state, and table[i][j] is the state you go to from table[i][0] with the j-th letter of the alphabet
	Utils::fillTable(table, *this);
	NFA dfa;
	dfa.addState(State(true, false, 0)); // the first state is initial
	for (size_t i = 0; i < table.getSize() - 1; i++) {
		dfa.addState(); // add the rest of the states from first column of table
	}

	//add the transitions (from table)
	for (size_t i = 0; i < table.getSize(); i++) {
		for (size_t j = 1; j < sizeof(alphabet) + 1; j++) {
			dfa.addTransition(Transition(dfa.states[i], dfa.states[Utils::findIndexInTable(i, j, table)], alphabet[j - 1]));
		}
	}

	//all new states(new states are vectors), containing final state from the nfa, are final
	for (size_t i = 0; i < stateCount; i++) {
		if (!states[i].isFinal()) {
			continue;
		}
		for (size_t j = 0; j < table.getSize(); j++) {
			if (table[j][0].containsAfter(i, 0)) {
				dfa.states[j] = State(dfa.states[j].isInitial(), true, j);
			}
		}
	}
	dfa.isDFA = true;
	dfa.regex = this->regex;
	*this = std::move(dfa);
}

// determinate, reverse, determinate, reverse, determinate in this order always gives the minimized automata 
//(Brzozowski theorem)
void NFA::minimise() {
	MyString regex = this->regex;
	this->determinate();
	Utils::reverseNFA(*this);
	this->determinate();
	Utils::reverseNFA(*this);
	this->determinate();
	this->regex = regex;
}

void NFA::makeTotal() {
	if (isDFA) {
		return;
	}

	addState(); // errorState
	size_t errorStateName = states[stateCount - 1].getName();

	for (size_t i = 0; i < stateCount; i++) {
		for (size_t j = 0; j < sizeof(alphabet); j++) {
			if (!Utils::transitionExists(states[i].getName(), alphabet[j], *this)) {
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
	isDFA = false;
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
	isDFA = false;
}

void NFA::addTransition(const Transition& other) {
	if (transitionCount == transitionCapacity) {
		resizeTransitions();
	}
	transitions[transitionCount] = other;
	transitionCount++;

	isDFA = false;
	this->regex = RegexParser::RegexFromNFA(*this);
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
	//fill visitedStates with initialStates
	MyVector<size_t> visitedStates;
	for (size_t i = 0; i < stateCount; i++) {
		if (states[i].isInitial()) {
			visitedStates.push_back(i);
		}
	}

	//for every state, add all states reachable from it (visitedStates.getSize() will increase with time)
	//after this loop, visitedStates will be filled with all reachable states
	for (size_t i = 0; i < visitedStates.getSize(); i++) {
		for (size_t j = 0; j < transitionCount; j++) {
			if (Utils::indexByName(transitions[j].getInitialState().getName(), *this) == visitedStates[i]) {
				if (!Utils::isInAlphabet(transitions[j].getTransitionValue())) { //we want to only check for the given alphabet(inside Constants.h)
					continue;
				}
				size_t toPush = Utils::indexByName(transitions[j].getResultState().getName(),*this);
				if (states[toPush].isFinal()) { //check if a final state is reached to avoid further calculations
					return false;
				}
				if (!visitedStates.containsAfter(toPush, 0)) {
					visitedStates.push_back(toPush);
				}
			}
		}
	}

	//if no reachable states are final, return true
	for (size_t i = 0; i < visitedStates.getSize(); i++) {
		if (states[visitedStates[i]].isFinal()) {
			return false;
		}
	}
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
	Utils::addEpsilonTransitionStates(indices, *this);
	//indices is now filled with all starting states(and those that are reachable from the starting states with epsilon transitions)

	//performs epsilon transition for every letter in the word and keeps the current states in indices after each step
	for (size_t i = 0; i < word.length(); i++) {
		if (indices.getSize() == 0) {
			return false;
		}
		Utils::deltaTransition(indices, word[i], *this);
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
	Utils::addElementsFrom(lhs, rhs);
	lhs.isDFA = false;
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

	Utils::addElementsFrom(lhs, rhs);

	//connect intermediate state to all initial states from rhs with E-transitions(rhs states start with intermediateStateIndex + 1)
	for (size_t i = intermediateStateIndex + 1; i < lhs.stateCount; i++) {
		if (lhs.states[i].isInitial()) {
			lhs.addTransition(Transition(lhs.states[intermediateStateIndex], lhs.states[i], EPSILON));
			lhs.states[i] = State(false, lhs.states[i].isFinal(), lhs.states[i].getName()); //make non-initial
		}
	}
	lhs.isDFA = false;
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
	nfa.isDFA = false;
}