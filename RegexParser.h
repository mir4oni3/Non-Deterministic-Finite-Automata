#pragma once
#include "NFA.h"
#include "MyVector.hpp"

struct RegexParser {
	static NFA NFAfromRegex(const MyString& regex);
	static MyString RegexFromNFA(const NFA& nfa); // doesn't work
private:
	static void regexToRPN(MyString& regex); // shunting yard algorithm ; RPN -> Reverse Polish Notation
	static NFA RPNtoNFA(const MyString& RPNregex); //RPN evaluation algorithm
	static NFA NFAfromLetter(const char letter);

	static MyString calcReg(size_t i, size_t j,size_t k, const NFA& nfa);
	
	RegexParser() = default; // this struct is uninstantiable since the default constructor is private
	RegexParser(const RegexParser&) = delete;
	RegexParser& operator=(const RegexParser&) = delete;
};