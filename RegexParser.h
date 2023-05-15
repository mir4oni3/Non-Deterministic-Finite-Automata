#pragma once
#include "NFA.h"

struct RegexParser {
	static NFA NFAfromRegex(const MyString& regex); 
	static MyString RegexFromNFA(const NFA& nfa);
private:
	static void regexToReversePolishNotation(MyString& regex); // shunting yard algorithm
	static NFA RPNtoNFA(const MyString& RPNregex); //RPN evaluation algorithm
	static NFA NFAfromLetter(const char letter);

	RegexParser() = default; // this struct is uninstantiable since the default constructor is private ("static class")
	RegexParser(const RegexParser&) = delete;
	RegexParser& operator=(const RegexParser&) = delete;
};