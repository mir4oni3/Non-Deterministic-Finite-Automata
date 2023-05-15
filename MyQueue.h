#pragma once
#include "MyString.h"

template <class T>
class MyQueue{
	T* elements = nullptr;
	size_t count = 0;
	size_t capacity = 0;

	void resize(); // downsize ?
	void copyFrom(const MyQueue& other);
	void free();
public:
	bool isEmpty() const;
	size_t getCount() const;

	void enqueue(const T& element);
	T dequeue();
	T peek() const;

	MyQueue();
	MyQueue& operator=(const MyQueue& other);
	MyQueue& operator=(MyQueue&& other);
	MyQueue(const MyQueue& other);
	MyQueue(MyQueue&& other);
	~MyQueue();
};