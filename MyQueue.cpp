#include "MyQueue.h"
#include "MyString.h"
#include <iostream>

template<class T>
void MyQueue<T>::copyFrom(const MyQueue<T>& other) {
	elements = new T[other.capacity];
	capacity = other.capacity;
	count = other.count;
	for (int i = 0; i < count; i++) {
		elements[i] = other.elements[i];
	}
}

template<class T>
void MyQueue<T>::free() {
	delete[] elements;
	count = capacity = 0;
	elements = nullptr;
}

template<class T>
MyQueue<T>& MyQueue<T>::operator=(const MyQueue<T>& other) {
	if (this != &other) {
		free();
		copyFrom(other);
	}
	return *this;
}

template<class T>
MyQueue<T>::MyQueue(const MyQueue<T>& other) {
	copyFrom(other);
}


template<class T>
MyQueue<T>::MyQueue(MyQueue&& other) : elements(other.elements), count(other.count), capacity(other.capacity) {
	other.elements = nullptr;
}

template<class T>
MyQueue<T>& MyQueue<T>::operator=(MyQueue&& other) {
	if (this != &other) {
		free();
		elements = other.elements;
		count = other.count;
		capacity = other.capacity;
		other.elements = nullptr;
	}
	return *this;
}

template <class T>
void MyQueue<T>::enqueue(const T& element) {
	if (count == capacity) {
		resize();
	}
	elements[count++] = element;
}

template <class T>
T MyQueue<T>::dequeue() {
	if (isEmpty()) {
		throw std::logic_error("Cannot dequeue from empty queue!");
	}
	T element = elements[0];
	for (size_t i = 0; i < count - 1; i++) {
		elements[i] = elements[i + 1];
	}
	count--;
	return element;
}

template <class T>
T MyQueue<T>::peek() const {
	if (isEmpty()) {
		throw std::logic_error("Cannot peek from empty queue!");
	}
	return elements[0];
}

template <class T>
MyQueue<T>::MyQueue(){
	elements = new T[initialDataStructureCapacity];
	this->capacity = initialDataStructureCapacity;
}

template <class T>
MyQueue<T>::~MyQueue() {
	free();
}

template<class T>
bool MyQueue<T>::isEmpty() const {
	return count == 0;
}

template<class T>
void MyQueue<T>::resize() {
	T* newElements = new T[2 * capacity];
	for (size_t i = 0; i < count; i++) {
		newElements[i] = elements[i];
	}
	delete[] elements;
	elements = newElements;
	capacity *= 2;
}

template<class T>
size_t MyQueue<T>::getCount() const {
	return count;
}