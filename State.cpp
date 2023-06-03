#include "State.h"
#include <iostream>

bool State::isInitial() const {
	return initial;
}

bool State::isFinal() const {
	return final;
}

State::State(bool isInitial, bool isFinal, unsigned name) {
	this->initial = isInitial;
	this->final = isFinal;
	this->name = name;
}

unsigned State::getName() const {
	return name;
}

void State::setName(unsigned name) {
	this->name = name;
}

void State::print() const {
	std::cout << this-> name << " " << (this->initial ? " initial" : "") << (this->final ? " final" : "") << std::endl;
}