#pragma once
#include <list>

using namespace std;

template <typename T>
class SuperCoolList
{
public:
	SuperCoolList();
	void add(T elem);
private:
	list<T> collection;
};
