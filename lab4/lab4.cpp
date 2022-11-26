#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>

#include <Windows.h>

#include "SuperCoolList.h"
#include "ThreadPool.h"

//using namespace std;
namespace fs = std::filesystem;

SuperCoolList<std::pair<fs::path, int>> *collection;

void task(fs::path path);

int main()
{
	setlocale(LC_ALL, "Russian");

	collection = new SuperCoolList<std::pair<fs::path, int>>();
	std::vector<fs::path> *paths = new std::vector<fs::path>();
    std::string path = "D:/student/3 курс 1 сем/ОСиСП/lab4/lab4/test/";

	for (auto& entry : fs::directory_iterator(path)) {
		const fs::path filename = entry.path().filename();
		if (filename.extension().string() == ".txt") {
			paths->emplace_back(path + filename.string());
		}
	}

	ThreadPool *pool = new ThreadPool(paths->size() / 4);
	for (int i = 0; i < paths->size(); i++) {
		pool->add_task(task, std::ref(paths->at(i)));
	}
	pool->wait_all();
	delete(pool);

	for (auto& item : *collection->collection) {
		std::cout << item.first << " " << item.second << std::endl;
	}
}

void task(fs::path path) {
	int counter = 0;
	std::string test = "test";
	ifstream file;
	file.open(path);
	
	while (!file.eof()) {
		int i = 0;
		while (file.get() == test[i]) {
			i++;
			if (i == test.size()) {
				counter++;
				break;
			}
			if (file.eof()) break;
		}
	}

	std::pair<fs::path, int> *temp = new std::pair<fs::path, int>(path.filename(), counter);
	collection->add(*temp);
}