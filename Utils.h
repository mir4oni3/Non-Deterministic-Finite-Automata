#pragma once
#include "MyStack.hpp"
#include "MyQueue.hpp"
#include "MyVector.hpp"
#include "NFA.h"

class Utils {
public:
	static bool isInAlphabet(char ch);
	static bool isValidRegexSymbol(char ch);

	static MyString toString(const MyStack<char> st);
	static MyString toString(const MyQueue<char> st);

	static size_t indexAt(unsigned value, unsigned* arr, size_t size);

	static void addElementsFrom(NFA& lhs, const NFA& rhs); // //adds all states and transitions from rhs to lhs

	static bool transitionExists(unsigned state, char ch, const NFA& nfa);

	static void deltaTransition(MyVector<size_t>& indices, char ch, const NFA& nfa);
	static void addEpsilonTransitionStates(MyVector<size_t>& indices, const NFA& nfa, size_t oldIndicesSize = 0);
	static size_t indexByName(size_t name, const NFA& nfa);

	static void makeSingleInitialAndFinal(NFA& nfa);

	static MyVector<size_t> getResultStates(const MyVector<size_t>& initial, char ch, const NFA& nfa);

	static void fillTable(MyVector<MyVector<MyVector<size_t>>>& table, const NFA& nfa, size_t row = 0);
	static void addNewRows(MyVector<MyVector<MyVector<size_t>>>& table,const MyVector<MyVector<size_t>>& row);
	static bool areEqual(const MyVector<size_t>& v1,const MyVector<size_t>& v2);
	static void initTable(MyVector<MyVector<MyVector<size_t>>>& table);
	static void initRow(MyVector<MyVector<size_t>>& row);
	static size_t findIndexInTable(size_t i, size_t j, const MyVector<MyVector<MyVector<size_t>>>& table);
	static void printTable(const MyVector<MyVector<MyVector<size_t>>>& table);

	static void reverseNFA(NFA& nfa);
};