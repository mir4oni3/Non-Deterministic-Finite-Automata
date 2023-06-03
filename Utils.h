#pragma once
#include "MyStack.hpp"
#include "MyQueue.hpp"
#include "MyVector.hpp"
#include "NFA.h"

bool isInAlphabet(char ch);
bool isValidRegexSymbol(char ch);

MyString toString(const MyStack<char> st);
MyString toString(const MyQueue<char> st);

size_t indexAt(unsigned value, unsigned* arr, size_t size);

void addElementsFrom(NFA& lhs, const NFA& rhs); // //adds all states and transitions from rhs to lhs

bool transitionExists(unsigned state, char ch, const NFA& nfa);

void deltaTransition(MyVector<size_t>& indices, char ch, const NFA& nfa);
void addEpsilonTransitionStates(MyVector<size_t>& indices, const NFA& nfa, size_t oldIndicesSize = 0);
size_t indexByName(size_t name, const NFA& nfa);