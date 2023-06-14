#include <iostream>
#include "RegexParser.h"
#include "Utils.h"

int main() {
	NFA nfa("(a+b)*.a.b.a+a*");
	nfa.minimise();
	nfa.print();
	/*nfa.print();
	std::cout << std::endl << std::endl;
	nfa2.print();*/
}