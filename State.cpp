#include "State.h"

bool State::isInitial() const {
	return initial;
}

bool State::isFinal() const {
	return final;
}

State::State(bool isInitial, bool isFinal) {
	this->initial = isInitial;
	this->final = isFinal;
}