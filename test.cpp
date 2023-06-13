#include <iostream>
#include "RegexParser.h"
#include "Utils.h"

int main() {
	NFA nfa("a.(b.a+b)*");
	std::cout << nfa.isEmptyLanguage() << std::endl;
	std::cout << nfa.accept("aba") << std::endl;
	std::cout << nfa.accept("baa") << std::endl;

	NFA t2("b.(a)*");
	Union(t2, nfa);
	std::cout << t2.accept("aba") << std::endl;
	std::cout << t2.accept("baa") << std::endl;
}