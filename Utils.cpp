#include "Utils.h"
#include "Constants.h"
#include <cstring>

bool isInAlphabet(char ch) {
	for (int i = 0; i < strlen(alphabet); i++) {
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
		str[i++] = st.pop();
	}
	MyString result(str);
	delete[] str;
	return result;
}

MyString toString(const MyQueue<char> queue) {
	MyQueue<char> q = queue;
	char* str = new char[2 * q.getCount()];
	int i = 0;
	while (!q.isEmpty()) {
		str[i++] = ' ';
		str[i++] = q.dequeue();
	}
	MyString result(str);
	delete[] str;
	return result;
}

bool isValidRegexSymbol(char ch) {
	return (isInAlphabet(ch) || ch == '(' || ch == ')' || ch == '*' || ch == '+' || ch == '.');
}