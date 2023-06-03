#pragma once

class State {
	bool initial = false;
	bool final = false;
	unsigned name = 0; //necessary label so that transitions differentiate different states in the NFA
public:
	bool isInitial() const;
	bool isFinal() const;

	State() = default;
	State(bool isInitial, bool isFinal, unsigned name);

	unsigned getName() const;
	void setName(unsigned name);
	void print() const;
};