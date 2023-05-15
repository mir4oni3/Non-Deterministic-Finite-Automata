#pragma once
#include "State.h"
#include "Transition.h"
#include "MyString.h"

class NFA {
	State* states = nullptr;
	size_t stateCount = 0;
	Transition* transitions = nullptr; 
	size_t transitionCount = 0;

	MyString regex;
	bool isDFA = false;

	void copyFrom(const NFA& other);
	void free();
public:

	NFA(const MyString& regex);
	NFA() = default;
	~NFA();
	NFA(const NFA& other);
	NFA(NFA&& other) noexcept;
	NFA& operator=(const NFA& other);
	NFA& operator=(NFA&& other) noexcept;


	void determinate();
	void minimise();
	void makeTotal();
	void addState(const State& other);
	void addTransition(const Transition& other);

	bool isEmptyLanguage() const;
	bool accept(const MyString& word) const; //can accept const char* due to MyString converting constructor

	State& operator[](const size_t index) const; // returns state at index
};

NFA Union(const NFA& lhs, const NFA& rhs);
void UnionRef(NFA& lhs, const NFA& rhs);
NFA Concat(const NFA& lhs, const NFA& rhs);
void ConcatRef(NFA& lhs, const NFA& rhs);
NFA Star(const NFA& nfa);
void StarRef(NFA& nfa);