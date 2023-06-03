#include "RegexParser.h"
#include <cstring>
#include "Constants.h"
#include "Utils.h"
#include "MyQueue.hpp"
#include "MyStack.hpp"
#include "MyVector.hpp"

NFA RegexParser::NFAfromRegex(const MyString& regex) {
	for (size_t i = 0; i < regex.length(); i++) {
		if (!isValidRegexSymbol(regex[i])) {
			throw std::invalid_argument("Error: invalid regex");
		}
	}
	MyString regexCopy(regex);
	regexToRPN(regexCopy);
	NFA result = std::move(RPNtoNFA(regexCopy));
	result.regex = regex;
	return result;
}

//RPN evaluation algorithm
NFA RegexParser::RPNtoNFA(const MyString& RPNregex) {
	MyStack<NFA> stack;

	for (size_t i = 1; i < RPNregex.length(); i += 2) {
		char currentSymbol = RPNregex[i];

		if (isInAlphabet(currentSymbol) || currentSymbol == EPSILON) {
			stack.push(RegexParser::NFAfromLetter(currentSymbol));
		}

		else if (currentSymbol == '*') {
			if (stack.getCount() < 1) {
				throw std::invalid_argument("Error: invalid regex");
			}
			NFA current = stack.peek();
			stack.pop();
			Star(current);
			stack.push(current);
		}

		else if (currentSymbol == '.' || currentSymbol == '+') {
			if (stack.getCount() < 2) {
				throw std::invalid_argument("Error: invalid regex");
			}
			NFA first = stack.peek();
			stack.pop();
			NFA second = stack.peek();
			stack.pop();
			(currentSymbol == '.') ? Concat(second, first) : Union(second, first);
			stack.push(second);
		}
	}

	if (stack.getCount() == 1) {
		return stack.peek();
	}
	else {
		throw std::invalid_argument("Error: invalid regex");
	}
}

//RPN -> Reverse Polish Notation
// shunting yard algorithm
void RegexParser::regexToRPN(MyString& regex) {
	MyStack<char> stack;
	MyQueue<char> queue;

	for (size_t i = 0; i < regex.length(); i++) {
		char currentElement = regex[i];

		if (currentElement == '(') {
			stack.push(currentElement);
		}

		else if (currentElement == ')') {
			char currentStackElement = '\0';
			while (true) {
				currentStackElement = stack.peek();
				stack.pop();
				if (currentStackElement == '(') {
					break;
				}
				queue.enqueue(currentStackElement);
			}
		}

		else if (isInAlphabet(currentElement) || currentElement == EPSILON) {
			queue.enqueue(currentElement);
		}

		else if (currentElement == '*') {
			stack.push(currentElement);
		}

		else if (currentElement == '.') {
			if (stack.isEmpty()) {
				stack.push(currentElement);
				continue;
			}
			char topOfStack = stack.peek();
			if (topOfStack == '*') {
				stack.pop();
				queue.enqueue(topOfStack);
			}
			stack.push(currentElement);
		}

		else if (currentElement == '+') {
			if (stack.isEmpty()) {
				stack.push(currentElement);
				continue;
			}
			char topOfStack = stack.peek();
			if (topOfStack == '*' || topOfStack == '.') {
				stack.pop();
				queue.enqueue(topOfStack);
			}
			stack.push(currentElement);
		}

	}

	while (!stack.isEmpty()) {
		queue.enqueue(stack.peek());
		stack.pop();
	}

	regex = std::move(toString(queue));
}

NFA RegexParser::NFAfromLetter(const char letter) {
	NFA result;
	if (letter == EPSILON) {
		result.addState(State(true, true, 0));
	}
	else {
		result.addState(State(true, false, 0));
		result.addState(State(false, true, 1));
		result.addTransition(Transition(result[0], result[1], letter));
	}

	result.regex = "E";
	result.regex[0] = letter;
	return result;
}

 MyString RegexParser::RegexFromNFA(const NFA& nfa) {
	 MyVector<size_t> initialIndices;
	 MyVector<size_t> finalIndices;
	 for (size_t i = 0; i < nfa.stateCount; i++) {
		 if (nfa.states[i].isInitial()) {
			 initialIndices.push_back(i);
		 }
		 if (nfa.states[i].isFinal()) {
			 finalIndices.push_back(i);
		 }
	 }

	 MyString result;
	 for (size_t i = 0; i < initialIndices.getSize(); i++) {
		 for (size_t j = 0; j < finalIndices.getSize(); j++) {
			 result += calcReg(initialIndices[i], finalIndices[j], nfa.stateCount, nfa) + "+";
		 }
	 }
	 return result;
}

 MyString RegexParser::calcReg(size_t i, size_t j, size_t k, const NFA& nfa) {
	 MyString result;
	 if (k > 0) {
		 //using kleene's first theorem
		 result = calcReg(i, j, k - 1, nfa) +"+(" +
				  calcReg(i, k - 1, k - 1, nfa) + ").(" + 
				  calcReg(k - 1, k - 1, k - 1, nfa) + ")*.(" +
				  calcReg(k - 1, j, k - 1, nfa) + ")";
	 }
	 //recursion base
	 if (k == 0) {
		 for (size_t t = 0; t < nfa.transitionCount; t++) {
			 if (nfa.transitions[t].getInitialState().getName() == i && nfa.transitions[t].getResultState().getName() == j) {
				 result += nfa.transitions[t].getTransitionValue() + "+";
			 }
		 }
		 if (i == j) {
			 MyString EpsilonString = "E";
			 EpsilonString[0] = EPSILON;
			 result += EpsilonString;
		 }
		 else {
			 if (result.length() > 0) {
				 result[result.length() - 1] = '\0';
			}
		 }
	}
	 return result;
 }