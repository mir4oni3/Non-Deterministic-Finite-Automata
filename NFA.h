#pragma once
#include "State.h"
#include "Transition.h"
#include "MyString.h"
#include "Constants.h"
#include "MyVector.hpp"

class NFA {
	State* states = nullptr;
	size_t stateCount = 0;
	size_t stateCapacity = minStateCapacity;

	Transition* transitions = nullptr;
	size_t transitionCount = 0;
	size_t transitionCapacity = minTransitionCapacity;

	MyString regex;
	bool isDFA = false;

	void resizeStates();
	void resizeTransitions();

	void copyFrom(const NFA& other);
	void moveFrom(NFA&& other);
	void free();
public:

	NFA(const MyString& regex);
	NFA();
	~NFA();
	NFA(const NFA& other);
	NFA(NFA&& other) noexcept;
	NFA& operator=(const NFA& other);
	NFA& operator=(NFA&& other) noexcept;

	void determinate();
	void minimise();
	void makeTotal();
	void addState(const State& other);
	void addState();
	void addTransition(const Transition& other);


	void print() const;

	bool isEmptyLanguage() const;
	bool accept(const MyString& word) const; //can accept const char* due to MyString converting constructor

	State& operator[](const size_t index) const; // returns state at index

	friend void Union(NFA& lhs, const NFA& rhs);
	friend void Concat(NFA& lhs, const NFA& rhs);
	friend void Star(NFA& nfa);

	friend void addElementsFrom(NFA& lhs, const NFA& rhs);
	friend bool transitionExists(unsigned state, char ch, const NFA& nfa);

	friend void deltaTransition(MyVector<size_t>& indices, char ch, const NFA& nfa);
	friend size_t indexByName(size_t name, const NFA& nfa);
	friend void addEpsilonTransitionStates(MyVector<size_t>& indices, const NFA& nfa, size_t oldIndicesSize);

	friend class RegexParser;
};

void Union(NFA& lhs, const NFA& rhs); // transforms lhs to lhs U rhs
void Concat(NFA& lhs, const NFA& rhs); // transforms lhs to rhs.lhs
void Star(NFA& nfa); // transforms nfa to (nfa)*