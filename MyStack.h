#pragma once
#include "MyString.h"

template<class T>
class MyStack {
	T* elements = nullptr;
	size_t count = 0;
	size_t capacity = 0;

	void resize(); // downsize ?
	void copyFrom(const MyStack& other);
	void free();
public:
	bool isEmpty() const;
	size_t getCount() const;

	void push(const T& element);
	T pop();
	T peek() const;

	MyStack();
	MyStack& operator=(const MyStack& other);
	MyStack& operator=(MyStack&& other);
	MyStack(const MyStack& other);
	MyStack(MyStack&& other);
	~MyStack();
};