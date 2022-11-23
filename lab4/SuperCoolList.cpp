#include "SuperCoolList.h"

template <typename T>
SuperCoolList<T>::SuperCoolList() {
	this->collection = new list<T>();
}

template <typename T>
inline void SuperCoolList<T>::add(T elem) {
	this->collection.push_back(elem);
}