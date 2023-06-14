#include "NFA.h"
#include "RegexParser.h"
#include "Constants.h"
#include "Utils.h"
#include <cstring>
#include <iostream>
#include <fstream>

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
	stateCount = stateCapacity = 0;
	transitionCount = transitionCapacity = 0;
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

//we save all the fields of nfa instead of only the regex, because 
//when we read the nfa, we don't want to run the regex->nfa algorithm again
void NFA::serialize(const MyString& filename) {
	std::ofstream ofs(filename.c_str(), std::ios::binary | std::ios::out);
	if (!ofs.is_open()) {
		throw std::exception("Couldn't open file");
	}
	//write states
	ofs.write((const char*)&stateCapacity, sizeof(stateCapacity));
	ofs.write((const char*)&stateCount, sizeof(stateCount));
	for (size_t i = 0; i < stateCount; i++) {
		Utils::serializeState(ofs, states[i]);
	}

	//write transitions
	ofs.write((const char*)&transitionCapacity, sizeof(transitionCapacity));
	ofs.write((const char*)&transitionCount, sizeof(transitionCount));
	for (size_t i = 0; i < transitionCount; i++) {
		Utils::serializeState(ofs, transitions[i].getInitialState());
		Utils::serializeState(ofs, transitions[i].getResultState());
		char transitionValue = transitions[i].getTransitionValue();
		ofs.write((const char*)&transitionValue, sizeof(transitionValue));
	}

	//write regex
	size_t regexLength = regex.length();
	ofs.write((const char*)&regexLength, sizeof(regexLength));
	ofs.write(regex.c_str(), regexLength);

	//write isDFA
	ofs.write((const char*)&isDFA, sizeof(isDFA));

	ofs.close();
}

//we save all the fields of nfa instead of only the regex, because 
//when we read the nfa, we don't want to run the regex->nfa algorithm again
void NFA::deserialize(const MyString& filename) {
	std::ifstream ifs(filename.c_str(), std::ios::binary | std::ios::in);
	if (!ifs.is_open()) {
		throw std::exception("Couldn't open file");
	}
	free();//clear previous automata data

	//read states
	ifs.read((char*)&stateCapacity, sizeof(stateCapacity));
	states = new State[stateCapacity];
	size_t tempStateCount = 0;
	ifs.read((char*)&tempStateCount, sizeof(tempStateCount));
	for (size_t i = 0; i < tempStateCount; i++) {
		addState(Utils::readState(ifs)); //increments stateCount, after loop stateCount will be set
	}

	//read transitions
	ifs.read((char*)&transitionCapacity, sizeof(transitionCapacity));
	transitions = new Transition[transitionCapacity];
	size_t tempTransitionCount = 0;
	ifs.read((char*)&tempTransitionCount, sizeof(tempTransitionCount));
	for (size_t i = 0; i < tempTransitionCount; i++) {
		State initial = Utils::readState(ifs);
		State result = Utils::readState(ifs);
		char transitionValue;
		ifs.read((char*)&transitionValue, sizeof(transitionValue));
		addTransition(Transition(initial, result, transitionValue)); //increments transitionCount, after loop transitionCount will be set
	}

	//read regex
	size_t regexLength;
	ifs.read((char*)&regexLength, sizeof(regexLength));
	char* regex = new char[regexLength + 1];
	ifs.read(regex, regexLength);
	regex[regexLength] = '\0';
	this->regex = MyString(regex);
	delete[] regex;

	//read isDFA
	ifs.read((char*)&isDFA, sizeof(isDFA));

	ifs.close();
}

bool NFA::isEmptyLanguage() const {
	//fill visitedStates with initialStates
	MyVector<size_t> visitedStates;
	for (size_t i = 0; i < stateCount; i++) {
		if (states[i].isInitial()) {
			visitedStates.push_back(i);
		}
	}

	//for every state, add all states reachable from it
	//after this loop, visitedStates will be filled with all reachable states
	for (size_t i = 0; i < visitedStates.getSize(); i++) { //  visitedStates.getSize() will increase over time
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

	//perform epsilon transition for every letter in the word and keep the current states in indices after each step
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

NFA Union(const NFA& lhs, const NFA& rhs) {
	NFA lhsCopy = lhs;
	UnionRef(lhsCopy, rhs);
	return lhsCopy;
}

NFA Concat(const NFA& lhs, const NFA& rhs) {
	NFA lhsCopy = lhs;
	ConcatRef(lhsCopy, rhs);
	return lhsCopy;
}

NFA Star(const NFA& nfa) {
	NFA copy = nfa;
	StarRef(copy);
	return copy;
}

NFA Complement(const NFA& nfa) {
	NFA nfaCopy(nfa);
	nfaCopy.minimise();
	for (size_t i = 0; i < nfaCopy.stateCount; i++) {
		nfaCopy.states[i] = State(nfaCopy.states[i].isInitial(), !nfaCopy.states[i].isFinal(), nfaCopy.states[i].getName());
	}

	nfaCopy.regex = RegexParser::RegexFromNFA(nfaCopy);
	return nfaCopy;
}

//Using DeMorgan's law,   lhs^rhs = !(!lhs U !rhs)
NFA Intersection(const NFA& lhs, const NFA& rhs) {
	NFA lhsComplement = Complement(lhs);
	NFA rhsComplement = Complement(rhs);
	UnionRef(lhsComplement, rhsComplement);
	return Complement(lhsComplement);
}

// L1-L2 = L1^(!L2)
NFA Difference(const NFA& lhs, const NFA& rhs) {
	NFA rhsComplement = Complement(rhs);
	return Intersection(lhs, rhsComplement);
}

void UnionRef(NFA& lhs, const NFA& rhs) { 
	MyString lhsRegex = lhs.regex;
	Utils::addElementsFrom(lhs, rhs);
	lhs.regex = lhsRegex + "+" + rhs.regex;
	lhs.isDFA = false;
}

void ConcatRef(NFA& lhs, const NFA& rhs) {
	MyString lhsRegex = lhs.regex;

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
	lhs.regex = lhsRegex + "." + rhs.regex;
}

void StarRef(NFA& nfa) {
	//add new state which is initial and final so that epsilon is added to the language
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
	nfa.regex = "(" + nfa.regex + ")*";
	nfa.isDFA = false;
}