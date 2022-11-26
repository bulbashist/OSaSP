#pragma once
#include <list>
#include <mutex>

template <typename T>
class SuperCoolList
{
public:
	SuperCoolList();
	void add(T elem);

	std::list<T> *collection;
private:
	std::mutex col_mtx;
};


template <typename T>
SuperCoolList<T>::SuperCoolList() {
	this->collection = new std::list<T>();
}

template <typename T>
inline void SuperCoolList<T>::add(T elem) {
	std::lock_guard<std::mutex> lock(this->col_mtx);
	this->collection->push_back(elem);
}