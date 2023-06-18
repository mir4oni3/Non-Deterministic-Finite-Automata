#include "RegexParser.h"
#include <cstring>
#include "Constants.h"
#include "Utils.h"
#include "MyQueue.hpp"
#include "MyStack.hpp"
#include "MyVector.hpp"

NFA RegexParser::NFAfromRegex(const MyString& regex) {
	for (size_t i = 0; i < regex.length(); i++) {
		if (!Utils::isValidRegexSymbol(regex[i])) {
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

		if (Utils::isInAlphabet(currentSymbol) || currentSymbol == EPSILON) {
			stack.push(RegexParser::NFAfromLetter(currentSymbol));
		}

		else if (currentSymbol == '*') {
			if (stack.getCount() < 1) {
				throw std::invalid_argument("Error: invalid regex");
			}
			NFA current = stack.peek();
			stack.pop();
			StarRef(current);
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
			(currentSymbol == '.') ? ConcatRef(second, first) : UnionRef(second, first);
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

		else if (Utils::isInAlphabet(currentElement) || currentElement == EPSILON) {
			queue.enqueue(currentElement);
		}

		else if (currentElement == '*') {
			stack.push(currentElement);
		}

		else if (currentElement == '.') {
			while (!stack.isEmpty() && (stack.peek() == '*' || stack.peek() == '.')) {
				queue.enqueue(stack.peek());
				stack.pop();
			}
			stack.push(currentElement);
		}

		else if (currentElement == '+') {
			while (!stack.isEmpty() && (stack.peek() == '*' || stack.peek() == '.' || stack.peek() == '+')) {
				queue.enqueue(stack.peek());
				stack.pop();
			}
			stack.push(currentElement);
		}

	}

	while (!stack.isEmpty()) {
		queue.enqueue(stack.peek());
		stack.pop();
	}

	regex = std::move(Utils::toString(queue));
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

//using kleene's first theorem
 MyString RegexParser::RegexFromNFA(const NFA& nfa) {
	 NFA nfaCopy(nfa);
	 nfaCopy.minimise();
	 MyString regex;

	 for (size_t i = 0; i < nfaCopy.stateCount; i++) {
		 if (nfaCopy.states[i].isFinal()) {
			 if (regex.length() != 0) {
				 regex += "+";
			 }
			 regex += calcReg(0, i, nfaCopy.stateCount, nfaCopy);
		 }
	 }

	 return regex;
}

 //using kleene's first theorem
 MyString RegexParser::calcReg(size_t i, size_t j, size_t k, const NFA& dfa) {
	 MyString regex;

	 //recursion base
	 if (k == 0) {
		 for (size_t ind = 0; ind < dfa.transitionCount; ind++) {
			 if (dfa.transitions[ind].getInitialState().getName() == i && dfa.transitions[ind].getResultState().getName() == j) {
				 if (regex.length() != 0) {
					 regex += "+";
				 }
				 MyString chStr = "E";
				 chStr[0] = dfa.transitions[ind].getTransitionValue();
				 regex += chStr;
			 }
		 }
		 if (i == j) {
			 if (regex.length() != 0) {
				 regex += "+";
			 }
			 MyString epsilon = "E";
			 epsilon[0] = EPSILON;
			 regex += epsilon;
		 }
	 }
	 else {
		 // We could do this
		 // regex += calcReg(i, j, k - 1, dfa) + "+(" + calcReg(i, k - 1, k - 1, dfa) + ").(" + calcReg(k - 1, k - 1, k - 1, dfa) + ")*.(" + calcReg(k - 1, j, k - 1, dfa) + ")";
		 // instead, but the regex will be filled with useless brackets and becomes very long and unreadable

		 MyString firstPart = calcReg(i, j, k - 1, dfa);
		 MyString secondPart = calcReg(i, k - 1, k - 1, dfa);
		 MyString thirdPart = calcReg(k - 1, k - 1, k - 1, dfa);
		 MyString fourthPart = calcReg(k - 1, j, k - 1, dfa);

		 size_t firstLen = firstPart.length();
		 size_t secondLen = secondPart.length();
		 size_t thirdLen = thirdPart.length();
		 size_t fourthLen = fourthPart.length();

		 regex += firstPart;
		 if (firstLen > 0 && secondLen > 0 && fourthLen > 0) {
			 regex += "+";
		 }
		 else {
			 return regex;
		 }

		 if (secondLen > 0) {
			 if (thirdLen > 0 || fourthLen > 0) {
				 if (!(secondLen == 1 && secondPart[0] == EPSILON)) {
					 regex += "(" + secondPart + ")";
				 }
			 }
			 else {
				 regex += secondPart;
			 }
		 }
		 if (secondLen > 0 && (thirdLen > 0 || fourthLen > 0) && secondPart[0] != EPSILON) {
			 regex += ".";
		 }
		 if (thirdLen > 0) {
			 if (thirdLen == 1) {
				 if (thirdPart[0] != EPSILON) {
					 regex += thirdPart + "*";
				 }
				 else if (!(secondLen > 0 || fourthLen > 0)) {
					 regex += thirdPart;
				 }
			 }
			 else {
				 regex += "(" + thirdPart + ")*";
			 }
		 }
		 if ((thirdLen > 1 || (thirdLen == 1 && thirdPart[0] != EPSILON)) && fourthLen > 0) {
			 regex += ".";
		 }

		 if (fourthLen == 1) {
			 regex += fourthPart;
		 }
		 else if (fourthLen > 1){
			 regex += "(" + fourthPart + ")";
		 }
	 }
	 return regex;
 }