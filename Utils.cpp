#include "Utils.h"
#include "Constants.h"
#include "NFA.h"
#include "MyVector.hpp"
#include "RegexParser.h"
#include <cstring>

bool Utils::isInAlphabet(char ch) {
	for (int i = 0; i < sizeof(alphabet); i++) {
		if (alphabet[i] == ch) {
			return true;
		}
	}
	return false;
}

MyString  Utils::toString(const MyStack<char> stack) {
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

MyString  Utils::toString(const MyQueue<char> queue) {
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

bool  Utils::isValidRegexSymbol(char ch) {
	return (Utils::isInAlphabet(ch) || ch == EPSILON || ch == '(' || ch == ')' || ch == '*' || ch == '+' || ch == '.');
}

size_t  Utils::indexAt(unsigned value, unsigned* arr, size_t r) {
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
void  Utils::addElementsFrom(NFA& lhs, const NFA& rhs) {
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

bool  Utils::transitionExists(unsigned state, char ch, const NFA& nfa) {
	for (size_t i = 0; i < nfa.transitionCount; i++) {
		if (nfa.transitions[i].getInitialState().getName() == state && nfa.transitions[i].getTransitionValue() == ch) {
			return true;
		}
	}
	return false;
}

size_t  Utils::indexByName(size_t name, const NFA& nfa) {
	for (size_t i = 0; i < nfa.stateCount; i++) {
		if (nfa.states[i].getName() == name) {
			return i;
		}
	}
}

void  Utils::addEpsilonTransitionStates(MyVector<size_t>& indices, const NFA& nfa, size_t oldIndicesSize) {
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

// ? addEpsilonTransitionStates should be called inside loop ??
void  Utils::deltaTransition(MyVector<size_t>& indices, char ch, const NFA& nfa) {
	addEpsilonTransitionStates(indices, nfa);
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

 void Utils::makeSingleInitialAndFinal(NFA& nfa) {
	 nfa.addState(State(true, false, 0)); // single initial state
	 nfa.addState(State(false, true, 0)); // single final state
	 for (size_t i = 0; i < nfa.stateCount - 2; i++) {
		 if (nfa.states[i].isInitial()) {
			 // make initial states non-initial
			nfa.states[i] = State(false, nfa.states[i].isFinal(), nfa.states[i].getName());
			 // add epsilon transition from new single initial state to ex-initial states
			 nfa.addTransition(Transition(nfa.states[nfa.stateCount - 2], nfa.states[i], EPSILON));
		 }
		 if (nfa.states[i].isFinal()) {
			 //make final states non-final
			 nfa.states[i] = State(nfa.states[i].isInitial(), false, nfa.states[i].getName());
			 // add epsilon transition from ex-final states to new single final state
			 nfa.addTransition(Transition(nfa.states[i], nfa.states[nfa.stateCount - 1], EPSILON));
		 }
	 }
}

 //state[stateCount - 1] is final and state[stateCount - 2] is initial
 MyVector<size_t> Utils::getResultStates(const MyVector<size_t>& initial, char ch, const NFA& nfa) {
	 MyVector<size_t> result = initial;
	 Utils::deltaTransition(result, ch, nfa);
	 
 }


 //	the transition table looks like this:
 // 
 //		[i][0]  [i][1]  [i][2]   [i][3]
 //			
 //		      |	   a	  b	       c	...
 //		--------------------------------
 //		vector| vector  vector  vector        <-   [0][j]
 //		vector| vector  vector  vector		  <-   [1][j]
 //		vector| vector  vector  vector		  <-   [2][j]
 //		vector| vector  vector  vector		  <-   [3][j]

 void Utils::fillTable(MyVector<MyVector<MyVector<size_t>>>& table, const NFA& nfa, size_t row) {
	 if (row == 0){
		 initTable(table);
		 initRow(table[0]);

		 table[0][0].push_back(indexByName(nfa.states[nfa.stateCount - 2].getName(), nfa)); // add initial state to table
		 addEpsilonTransitionStates(table[0][0], nfa); // add all states reached with epsilon from start state

		 //set the first index of the new starting state(which is the index of the old starting state) to the last index
		 //this leaves only the states which are reachable with epsilon from the starting state(without the starting state itself)
		 table[0][0][0] = table[0][0][table[0][0].getSize() - 1];
	 }
	 if (row >= table.getSize()) {
		 return;
	 }

	 for (size_t i = 0; i < table[row][0].getSize(); i++) {
		 for (size_t j = 0; j < sizeof(alphabet); j++) {
			 for (size_t k = 0; k < nfa.transitionCount; k++) {
				 if (nfa.transitions[k].getTransitionValue() == alphabet[j] && indexByName(nfa.transitions[k].getInitialState().getName(), nfa) == table[row][0][i]) {
					 table[row][j + 1].push_back(indexByName(nfa.transitions[k].getResultState().getName(), nfa));
					 addEpsilonTransitionStates(table[row][j + 1], nfa);
				 }
			 }
		 }
	 }

	 addNewRows(table, table[row]);

	 fillTable(table, nfa, row + 1);
 }

 void Utils::addNewRows(MyVector<MyVector<MyVector<size_t>>>& table,const MyVector<MyVector<size_t>>& row) {
	 for (size_t i = 1; i < sizeof(alphabet) + 1; i++) {
		 bool toAdd = true;
		 for (size_t j = 0; j < table.getSize(); j++) {
			 if (Utils::areEqual(table[j][0], row[i])) { // check if two vectors have the same elements, order doesn't matter
				 toAdd = false;
				 break;
			 }
		 }
		 if (toAdd) {
			 MyVector<MyVector<size_t>> newRow;
			 initRow(newRow);
			 table.push_back(newRow);
			 table[table.getSize() - 1][0] = row[i];
		 }
	 }
 }

 //order doesn't matter, duplicate elements are allowed
 bool Utils::areEqual(const MyVector<size_t>& v1, const MyVector<size_t>& v2) {
	 //check if every element in v1 is in v2
	 for (size_t i = 0; i < v1.getSize(); i++) {
		 bool found = false;
		 for (size_t j = 0; j < v2.getSize(); j++) {
			 if (v1[i] == v2[j]) {
				 found = true;
				 break;
			 }
		 }
		 if (!found) {
			 return false;
		 }
	 }
	 //check if every element in v2 is in v1
	 for (size_t i = 0; i < v2.getSize(); i++) {
		 bool found = false;
		 for (size_t j = 0; j < v1.getSize(); j++) {
			 if (v2[i] == v1[j]) {
				 found = true;
				 break;
			 }
		 }
		 if (!found) {
			 return false;
		 }
	 }
	 return true;
 }

 void Utils::initTable(MyVector<MyVector<MyVector<size_t>>>& table) {
	 MyVector<MyVector<size_t>> temp;
	 table.push_back(temp);
 }

 void Utils::initRow(MyVector<MyVector<size_t>>& row) {
	 for (size_t i = 0; i < sizeof(alphabet) + 1; i++) {
		 MyVector<size_t> temp1;
		 row.push_back(temp1);
	 }
 }

 size_t Utils::findIndexInTable(size_t i, size_t j, const MyVector<MyVector<MyVector<size_t>>>& table) {
	 for (size_t k = 0; k < table.getSize(); k++) {
		 if (Utils::areEqual(table[i][j], table[k][0])) {
			 return k;
		 }
	 }
 }

 void Utils::reverseNFA(NFA& nfa) {
	 for (size_t i = 0; i < nfa.transitionCount; i++) {
		 //flip transition so that the result state is starting and starting is result
		 nfa.transitions[i] = Transition(nfa.transitions[i].getResultState(), nfa.transitions[i].getInitialState(), nfa.transitions[i].getTransitionValue());
	 }
	 for (size_t i = 0; i < nfa.stateCount; i++) {
		 bool isFinal = nfa.states[i].isFinal();
		 bool isInitial = nfa.states[i].isInitial();
		 if (isInitial && isFinal) {
			 continue;
		 }
		 else if (isInitial) {
			 nfa.states[i] = State(false, true, nfa.states[i].getName());
		 }
		 else if (isFinal) {
			 nfa.states[i] = State(true, false, nfa.states[i].getName());
		 }
	 }

	 nfa.isDFA = false;
	 nfa.regex = RegexParser::RegexFromNFA(nfa);
 }

 void Utils::printTable(const MyVector<MyVector<MyVector<size_t>>>& table) {
	 for (int i = 0; i < sizeof(alphabet); i++) {
		 std::cout << alphabet[i] << " ";
	 }
	 std::cout << std::endl;

	 for (size_t i = 0; i < table.getSize(); i++) {
		 for (size_t j = 0; j < table[i].getSize(); j++) {
			 for (size_t k = 0; k < table[i][j].getSize(); k++) {
				 std::cout << table[i][j][k] << ",";
			 }
			 std::cout << " ";
		 }
		 std::cout << std::endl;
	 }
 }