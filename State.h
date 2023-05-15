#pragma once

class State {
	bool initial = false;
	bool final = false;
public:
	bool isInitial() const;
	bool isFinal() const;
	State() = default;
	State(bool isInitial, bool isFinal);
};