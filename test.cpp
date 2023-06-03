#include <iostream>
#include "RegexParser.h"

int main() {
	MyString str = "a.a.(a+b)*.b";
	NFA nfa(str);
	std::cout << nfa.accept("ababbbabbab");
}