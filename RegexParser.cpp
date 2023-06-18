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

 MyString RegexParser::RegexFromNFA(const NFA& nfa) {
	 return nfa.regex;
}