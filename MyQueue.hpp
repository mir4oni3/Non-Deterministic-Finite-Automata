#pragma once
#include <iostream>
#include <exception>
#include "Constants.h"

template <class T>
class MyQueue {
	T* data = nullptr;
	size_t capacity = 0;
	size_t size = 0;

	size_t get = 0;
	size_t put = 0;

	void resize();

	void moveFrom(MyQueue<T>&& other);
	void copyFrom(const MyQueue<T>& other);
	void free();
public:
	MyQueue();

	MyQueue(const MyQueue<T>& other);
	MyQueue<T>& operator=(const MyQueue<T>& other);

	MyQueue(MyQueue<T>&& other);
	MyQueue<T>& operator=(MyQueue<T>&& other);

	void enqueue(const T& obj);
	void enqueue(T&& obj);
	void dequeue();
	const T& peek() const;

	bool isEmpty() const;
	size_t getCount() const;

	~MyQueue();

};

template <class T>
MyQueue<T>::MyQueue() {
	capacity = initialDataStructureCapacity;
	data = new T[capacity];
	size = 0;
	get = put = 0;
}

template <class T>
void MyQueue<T>::enqueue(const T& obj) {
	if (size == capacity)
		resize();

	data[put] = obj;
	(++put) %= capacity;
	size++;
}

template <class T>
void MyQueue<T>::enqueue(T&& obj) {
	if (size == capacity)
		resize();

	data[put] = std::move(obj);
	(++put) %= capacity;
	size++;
}

template <class T>
bool MyQueue<T>::isEmpty() const {
	return size == 0;
}

template <class T>
size_t MyQueue<T>::getCount() const {
	return size;
}

template <class T>
const T& MyQueue<T>::peek() const {
	if (isEmpty())
		throw std::logic_error("Empty queue!");

	return data[get];
}

template <class T>
void MyQueue<T>::dequeue() {
	if (isEmpty())
		throw std::logic_error("Empty queue!");
	(++get) %= capacity;
	size--;
}

template <class T>
void MyQueue<T>::resize() {
	T* resizedData = new T[capacity * 2];
	for (size_t i = 0; i < size; i++)
	{
		resizedData[i] = data[get];
		(++get) %= capacity;
	}
	capacity *= 2;
	delete[] data;
	data = resizedData;
	get = 0;
	put = size;
}


template <class T>
void MyQueue<T>::copyFrom(const MyQueue<T>& other) {
	data = new T[other.capacity];
	for (int i = 0; i < other.capacity; i++)
		data[i] = other.data[i];

	get = other.get;
	put = other.put;

	size = other.size;
	capacity = other.capacity;

}

template <class T>
void MyQueue<T>::moveFrom(MyQueue<T>&& other) {
	get = other.get;
	put = other.put;

	size = other.size;
	capacity = other.capacity;

	data = other.data;
	other.data = nullptr;

	other.size = other.capacity = 0;
}

template <class T>
void MyQueue<T>::free() {
	delete[] data;
}

template <class T>
MyQueue<T>::MyQueue(const MyQueue<T>& other) {
	copyFrom(other);

}

template <class T>
MyQueue<T>& MyQueue<T>::operator=(const MyQueue<T>& other) {
	if (this != &other)
	{
		free();
		copyFrom(other);
	}
	return *this;
}

template <class T>
MyQueue<T>::MyQueue(MyQueue<T>&& other) {
	moveFrom(std::move(other));
}

template <class T>
MyQueue<T>& MyQueue<T>::operator=(MyQueue<T>&& other) {
	if (this != &other)
	{
		free();
		moveFrom(std::move(other));
	}
	return *this;
}

template <class T>
MyQueue<T>::~MyQueue() {
	free();
}