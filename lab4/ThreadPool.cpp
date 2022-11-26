#pragma once
#include "ThreadPool.h"

#include <thread>
#include <iostream>

ThreadPool::ThreadPool(int num_threads) {
	this->threads.reserve(num_threads);
	for (int i = 0; i < num_threads; i++) {
		this->threads.emplace_back(std::thread(&ThreadPool::run, this));
	}
}

void ThreadPool::run() {
    while (!quite) {
        std::unique_lock<std::mutex> lock(this->q_mtx);
        // std::cout << std::this_thread::get_id() << std::endl;
        // если есть задачи, то берём задачу, иначе - засыпаем
        this->q_cv.wait(lock, [this]()->bool { return !this->queue.empty() || quite; });

        if (!queue.empty()) {
            auto elem = std::move(queue.front());
            queue.pop();
            lock.unlock();

            //Костыльный вызов функции
            elem.first.get();

            std::lock_guard<std::mutex> lock2(this->completed_task_ids_mtx);
            completed_task_ids.insert(elem.second);
            completed_task_ids_cv.notify_all();
        }
    }
}

void ThreadPool::wait_all() {
    std::unique_lock<std::mutex> lock(this->q_mtx);

    // ожидаем вызова notify в функции run (сработает после завершения задачи)
    this->completed_task_ids_cv.wait(lock, [this]()->bool {
        std::lock_guard<std::mutex> task_lock(this->completed_task_ids_mtx);
        return this->queue.empty() && last_idx == this->completed_task_ids.size();
    });
}

ThreadPool::~ThreadPool() {
    quite = true;
    for (int i = 0; i < threads.size(); i++) {
        q_cv.notify_all();
        threads[i].join();
    }
}