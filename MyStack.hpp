#pragma once
#include "MyStack.hpp"
#include "Constants.h"
#include "MyString.h"
#include <exception>
#include <iostream>

template<class T>
class MyStack {
	T* elements = nullptr;
	size_t count = 0;
	size_t capacity = 0;

	void resize();

	void moveFrom(MyStack&& other);
	void copyFrom(const MyStack& other);
	void free();
public:
	bool isEmpty() const;
	size_t getCount() const;

	void push(const T& element);
	void push(T&& element);
	void pop();
	const T& peek() const;

	MyStack();
	MyStack& operator=(const MyStack& other);
	MyStack& operator=(MyStack&& other);
	MyStack(const MyStack& other);
	MyStack(MyStack&& other);
	~MyStack();
};

template<class T>
void MyStack<T>::moveFrom(MyStack<T>&& other) {
	elements = other.elements;
	other.elements = nullptr;
	count = other.count;
	capacity = other.capacity;
}

template<class T>
void MyStack<T>::copyFrom(const MyStack<T>& other) {
	elements = new T[other.capacity];
	capacity = other.capacity;
	count = other.count;
	for (int i = 0; i < count; i++) {
		elements[i] = other.elements[i];
	}
}

template<class T>
void MyStack<T>::free() {
	delete[] elements;
	count = capacity = 0;
	elements = nullptr;
}

template<class T>
MyStack<T>& MyStack<T>::operator=(const MyStack<T>& other) {
	if (this != &other) {
		free();
		copyFrom(other);
	}
	return *this;
}

template<class T>
MyStack<T>::MyStack(const MyStack<T>& other) {
	copyFrom(other);
}

template<class T>
MyStack<T>::MyStack(MyStack&& other) {
	moveFrom(std::move(other));
}

template<class T>
MyStack<T>& MyStack<T>::operator=(MyStack&& other) {
	if (this != &other) {
		free();
		moveFrom(std::move(other));
	}
	return *this;
}

template<class T>
bool MyStack<T>::isEmpty() const {
	return count == 0;
}

template <class T>
void MyStack<T>::push(const T& element) {
	if (count == capacity) {
		resize();
	}
	elements[count++] = element;
}

template <class T>
void MyStack<T>::push(T&& element) {
	if (count == capacity) {
		resize();
	}
	elements[count++] = std::move(element);
}

template <class T>
void MyStack<T>::pop() {
	if (isEmpty()) {
		throw std::logic_error("Cannot pop from empty stack!");
	}
	--count;
}

template <class T>
const T& MyStack<T>::peek() const {
	if (isEmpty()) {
		throw std::logic_error("Cannot peek from empty stack!");
	}
	return elements[count - 1];
}

template<class T>
void MyStack<T>::resize() {
	T* newElements = new T[2 * capacity];
	for (size_t i = 0; i < capacity; i++) {
		newElements[i] = elements[i];
	}
	delete[] elements;
	elements = newElements;
	capacity *= 2;
}

template <class T>
MyStack<T>::MyStack() {
	elements = new T[initialDataStructureCapacity];
	this->capacity = initialDataStructureCapacity;
}

template<class T>
MyStack<T>::~MyStack() {
	free();
}

template<class T>
size_t MyStack<T>::getCount() const {
	return count;
}