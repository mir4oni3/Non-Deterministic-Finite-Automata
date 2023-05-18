#include "RegexParser.h"
#include <cstring>
#include "Constants.h"
#include "Utils.h"
#include "MyQueue.hpp"
#include "MyStack.hpp"

NFA RegexParser::NFAfromRegex(const MyString& regex) {
	for (size_t i = 0; i < regex.length(); i++) {
		if (!isValidRegexSymbol(regex[i])) {
			throw std::invalid_argument("Error: invalid regex");
		}
	}
	MyString regexCopy(regex);
	regexToRPN(regexCopy);
	return RPNtoNFA(regexCopy);
}

//RPN evaluation algorithm
NFA RegexParser::RPNtoNFA(const MyString& RPNregex) {
	MyStack<NFA> stack;

	for (size_t i = 0; i < RPNregex.length(); i += 2) {
		char currentSymbol = RPNregex[i];

		if (isInAlphabet(currentSymbol)) {
			stack.push(RegexParser::NFAfromLetter(currentSymbol));
		}

		else if (currentSymbol == '*') {
			if (stack.getCount() < 1) {
				throw std::invalid_argument("Error: invalid regex");
			}
			NFA current = stack.pop();
			StarRef(current);
			stack.push(current);
		}

		else if (currentSymbol == '.' || currentSymbol == '+') {
			if (stack.getCount() < 2) {
				throw std::invalid_argument("Error: invalid regex");
			}
			NFA first = stack.pop();
			NFA second = stack.pop();
			(currentSymbol == '.') ? ConcatRef(second, first) : UnionRef(second, first);
			stack.push(second);
		}

	}

	if (stack.getCount() == 1) {
		return stack.pop();
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
			while (currentStackElement != '(') {
				currentStackElement = stack.pop();
				queue.enqueue(currentStackElement);
			}
			stack.pop();
		}

		else if (isInAlphabet(currentElement) || currentElement == EPSILON) {
			queue.enqueue(currentElement);
		}

		else if (currentElement == '*') {
			stack.push(currentElement);
		}

		else if (currentElement == '.') {
			char topOfStack = stack.peek();
			if (topOfStack == '*') {
				stack.pop();
				queue.enqueue(topOfStack);
				stack.push(currentElement);
			}
		}

		else if (currentElement == '+') {
			char topOfStack = stack.peek();
			if (topOfStack == '*' || topOfStack == '.') {
				stack.pop();
				queue.enqueue(topOfStack);
				stack.push(currentElement);
			}
		}

	}

	while (!stack.isEmpty()) {
		queue.enqueue(stack.pop());
	}

	regex = std::move(toString(queue));
}

NFA RegexParser::NFAfromLetter(const char letter) {
	NFA result;
	if (letter == EPSILON) {
		result.addState(State(true, true));
	}
	else {
		result.addState(State(true, false));
		result.addState(State(false, true));
		result.addTransition(Transition(result[0], result[1], letter));
	}
	return result;
}