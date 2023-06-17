#pragma once
#include "NFA.h"

class NFATool {
	 MyVector<NFA> automatas;
public:
	 void run();
private:
	bool isValidKey(int key);
	void add();
	void select();
	void remove();
	void kleeneStar();
	void complement();
	void intersect();
	void difference();
	void unite();
	void concatenation();

	int getFirstIndex();
	int getSecondIndex();
	int getChoice();
};