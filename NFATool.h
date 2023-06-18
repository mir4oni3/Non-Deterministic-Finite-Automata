#pragma once
#include "NFA.h"

class NFATool {
	static MyVector<NFA> automatas;
public:
	static void run();
private:
	static bool isValidKey(int key);
	static void add();
	static void select();
	static void remove();
	static void kleeneStar();
	static void complement();
	static void intersect();
	static void difference();
	static void unite();
	static void concatenation();

	static int getFirstIndex();
	static int getSecondIndex();
	static int getChoice();

	NFATool() = default; // this class is uninstantiable since the default constructor is private
	NFATool(const NFATool&) = delete;
	NFATool& operator=(const NFATool&) = delete;
};