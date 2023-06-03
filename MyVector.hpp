#pragma once

template <typename T>
class MyVector {
	T* elements;
	size_t count;
	size_t capacity;

	void resize();
	void copyFrom(const MyVector<T>& other);
	void moveFrom(MyVector<T>&& other);
	void free();
public:
	MyVector();
	MyVector(const MyVector<T>& other);
	MyVector(MyVector<T>&& other);
	MyVector<T>& operator=(const MyVector<T>& other);
	MyVector<T>& operator=(MyVector<T>&& other);
	~MyVector();

	MyVector(const T& fillElement);

	void push_back(const T& element);
	void push_back(T&& element);
	void pop_back();

	T operator[](size_t at) const;
	T& operator[](size_t at);

	bool isEmpty() const;
	size_t getSize() const;
	size_t getCapacity() const;

	bool containsAfter(const T& element,size_t index) const;

	void clear();
};

template <typename T>
void MyVector<T>::resize() {
	T* newElements = new T[2 * capacity];
	for (size_t i = 0; i < capacity; i++) {
		newElements[i] = elements[i];
	}
	delete[] elements;
	elements = newElements;
	capacity *= 2;
}

template <typename T>
void MyVector<T>::copyFrom(const MyVector<T>& other) {
	elements = new T[other.capacity];
	capacity = other.capacity;
	count = other.count;
	for (size_t i = 0; i < count; i++) {
		elements[i] = other.elements[i];
	}
}

template <typename T>
void MyVector<T>::moveFrom(MyVector<T>&& other) {
	capacity = other.capacity;
	count = other.count;
	elements = other.elements;
	other.elements = nullptr;
}

template <typename T>
void MyVector<T>::free() {
	delete[] elements;
	elements = nullptr;
}

template <typename T>
MyVector<T>::MyVector() : count(0), capacity(8) {
	elements = new T[capacity];
}

template <typename T>
MyVector<T>::MyVector(const MyVector<T>& other) {
	copyFrom(other);
}

template <typename T>
MyVector<T>::MyVector(MyVector<T>&& other) {
	moveFrom(other);
}

template <typename T>
MyVector<T>& MyVector<T>::operator=(const MyVector<T>& other) {
	if (this != &other) {
		free();
		copyFrom(other);
	}
	return *this;
}

template <typename T>
MyVector<T>& MyVector<T>::operator=(MyVector<T>&& other) {
	if (this != &other) {
		free();
		moveFrom(std::move(other));
	}
	return *this;
}

template <typename T>
MyVector<T>::~MyVector() {
	free();
}

template <typename T>
MyVector<T>::MyVector(const T& fillElement) : count(0), capacity(8) {
	elements = new T[capacity];
	for (size_t i = 0; i < capacity; i++) {
		elements[i] = fillElement;
	}
}

template <typename T>
void MyVector<T>::push_back(const T& element) {
	if (count == capacity) {
		resize();
	}
	elements[count++] = element;
}

template <typename T>
void MyVector<T>::push_back(T&& element) {
	if (count == capacity) {
		resize();
	}
	elements[count++] = std::move(element);
}

template <typename T>
void MyVector<T>::pop_back() {
	if (count < 1) {
		throw std::logic_error("Error! No items to pop.");
	}
	count--;
}

template <typename T>
size_t MyVector<T>::getCapacity() const {
	return capacity;
}

template <typename T>
T MyVector<T>::operator[](size_t at) const {
	if (at >= count) {
		throw std::invalid_argument("Error! Illegal index.");
	}
	return elements[at];
}

template <typename T>
T& MyVector<T>::operator[](size_t at) {
	if (at >= count) {
		throw std::invalid_argument("Error! Illegal index.");
	}
	return elements[at];
}

template <typename T>
bool MyVector<T>::isEmpty() const {
	return count == 0;
}

template <typename T>
size_t MyVector<T>::getSize() const {
	return count;
}

template <typename T>
bool MyVector<T>::containsAfter(const T& element, size_t index) const {
	if (index >= count) {
		return false;
	}
	for (size_t i = index; i < count; i++) {
		if (elements[i] == element) {
			return true;
		}
	}
	return false;
}

template <typename T>
void MyVector<T>::clear() {
	count = 0;
}
