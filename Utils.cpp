#include "Utils.h"
#include "Constants.h"
#include "NFA.h"
#include "MyVector.hpp"
#include <cstring>

bool isInAlphabet(char ch) {
	for (int i = 0; i < sizeof(alphabet); i++) {
		if (alphabet[i] == ch) {
			return true;
		}
	}
	return false;
}

MyString toString(const MyStack<char> stack) {
	MyStack<char> st = stack;
	char* str = new char[2 * st.getCount()];
	int i = 0;
	while (!st.isEmpty()) {
		str[i++] = ' ';
		str[i++] = st.peek();
		st.pop();
	}
	MyString result(str);
	delete[] str;
	return result;
}

MyString toString(const MyQueue<char> queue) {
	MyQueue<char> q = queue;
	char* str = new char[2 * q.getCount() + 1];
	int i = 0;
	while (!q.isEmpty()) {
		str[i++] = ' ';
		str[i++] = q.peek();
		q.dequeue();
	}
	str[i] = '\0';
	MyString result(str);
	delete[] str;
	return result;
}

bool isValidRegexSymbol(char ch) {
	return (isInAlphabet(ch) || ch == EPSILON || ch == '(' || ch == ')' || ch == '*' || ch == '+' || ch == '.');
}

size_t indexAt(unsigned value, unsigned* arr, size_t r) {
	r--;
	size_t l = 0;
	while (l <= r) {
		size_t m = (r + l) / 2;
		if (arr[m] == value) {
			return m;
		}
		if (arr[m] < value) {
			l = m + 1;
		}
		else {
			r = m - 1;
		}
	}
	return -1;
}

//adds all states and transitions from rhs to lhs
void addElementsFrom(NFA& lhs, const NFA& rhs) {
	unsigned* oldNames = new unsigned[rhs.stateCount];
	unsigned* newNames = new unsigned[rhs.stateCount]; // avoiding name collision


	//add rhs's states to lhs and fill oldNames and newNames
	for (size_t i = 0; i < rhs.stateCount; i++) {
		oldNames[i] = rhs.states[i].getName();
		lhs.addState(rhs.states[i]);
		newNames[i] = lhs.states[lhs.stateCount - 1].getName();
	}

	for (size_t i = 0; i < rhs.transitionCount; i++) {
		lhs.addTransition(rhs.transitions[i]);

		//search if the new transition's name is in the oldNames array and replaces it with the corresponding new name (name collision prevention)
		size_t index = indexAt(rhs.transitions[i].getInitialState().getName(), oldNames, rhs.stateCount); //binary search (names of states are sorted)
		if (index != -1) {
			lhs.transitions[lhs.transitionCount - 1].setInitialStateName(newNames[index]);
		}

		//same for result state
		index = indexAt(rhs.transitions[i].getResultState().getName(), oldNames, rhs.stateCount);
		if (index != -1) {
			lhs.transitions[lhs.transitionCount - 1].setResultStateName(newNames[index]);
		}
	}
}

bool transitionExists(unsigned state, char ch, const NFA& nfa) {
	for (size_t i = 0; i < nfa.transitionCount; i++) {
		if (nfa.transitions[i].getInitialState().getName() == state && nfa.transitions[i].getTransitionValue() == ch) {
			return true;
		}
	}
	return false;
}

size_t indexByName(size_t name, const NFA& nfa) {
	for (size_t i = 0; i < nfa.stateCount; i++) {
		if (nfa.states[i].getName() == name) {
			return i;
		}
	}
}

void addEpsilonTransitionStates(MyVector<size_t>& indices, const NFA& nfa, size_t oldIndicesSize) {
	size_t indicesSize = indices.getSize();
	for (size_t i = 0; i < indicesSize; i++) {
		for (size_t j = 0; j < nfa.transitionCount; j++) {
			if (indexByName(nfa.transitions[j].getInitialState().getName(), nfa) == indices[i]) {
				size_t resultState = indexByName(nfa.transitions[j].getResultState().getName(), nfa);
				if (nfa.transitions[j].getTransitionValue() == EPSILON && !indices.containsAfter(resultState,0)) {
					indices.push_back(resultState);
				}
			}

		}
	}
	if (oldIndicesSize != indices.getSize()) { // recursion stops when no new states are added
		//adding all states that are reached with epsilon from states reached with epsilon(and so on...) from indices recursively
		addEpsilonTransitionStates(indices, nfa, indices.getSize());
	}
}

void deltaTransition(MyVector<size_t>& indices, char ch, const NFA& nfa) {
	//adds all states to the end of indices that are reached by states in indices with the letter ch with one step (doesn't add epsilon transitions)
	size_t indicesSize = indices.getSize();
	for (size_t i = 0; i < indicesSize; i++) {
		for (size_t j = 0; j < nfa.transitionCount; j++) {
			if (nfa.transitions[j].getInitialState().getName() == nfa.states[indices[i]].getName() &&
			   (nfa.transitions[j].getTransitionValue() == ch)) {
				size_t toPush = indexByName(nfa.transitions[j].getResultState().getName(), nfa);
				if (!indices.containsAfter(toPush, indicesSize)) {
					indices.push_back(toPush);
				}
			}
		}
	}

	//removes all states in indices that are from the previous delta transition
	size_t newIndicesSize = indices.getSize();
	for (size_t i = indicesSize; i < newIndicesSize; i++) {
		indices[i - indicesSize] = indices[i];
	}
	for (size_t i = 0; i <indicesSize; i++) {
		indices.pop_back();
	}
	addEpsilonTransitionStates(indices, nfa); // adds recursively all states that are reached with epsilon from states in indices
}