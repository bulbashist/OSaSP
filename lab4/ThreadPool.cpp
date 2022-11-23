#include "ThreadPool.h"

ThreadPool::ThreadPool(int num_threads) {
	this->threads.reserve(num_threads);
	for (int i = 0; i < num_threads; i++) {
		this->threads.emplace_back(&ThreadPool::run, this);
	}
}

void ThreadPool::run() {
    while (!quite) {
        std::unique_lock<std::mutex> lock(q_mtx);

        // если есть задачи, то берём задачу, иначе - засыпаем
        // если мы зашли в деструктор, то quite будет true и мы не будем 
        // ждать завершения всех задач и выйдем из цикла
        this->q_cv.wait(lock, [this]()->bool { return !this->q.empty() || quite; });

        if (!q.empty()) {
            auto elem = std::move(q.front());
            q.pop();
            lock.unlock();

            // вычисляем объект типа std::future (вычисляем функцию) 
            elem.first.get();

            std::lock_guard<std::mutex> lock(completed_task_ids_mtx);

            // добавляем номер выполненой задачи в список завершённых
            completed_task_ids.insert(elem.second);

            // делаем notify, чтобы разбудить потоки
            completed_task_ids_cv.notify_all();
        }
    }
}



void ThreadPool::wait(int task_id) {
    std::unique_lock<std::mutex> lock(completed_task_ids_mtx);

    // ожидаем вызова notify в функции run (сработает после завершения задачи)
    completed_task_ids_cv.wait(lock, [this, task_id]()->bool {
        return completed_task_ids.find(task_id) != completed_task_ids.end();
        });
}

void ThreadPool::wait_all() {
    std::unique_lock<std::mutex> lock(this->q_mtx);

    // ожидаем вызова notify в функции run (сработает после завершения задачи)
    this->completed_task_ids_cv.wait(lock, [this]()->bool {
        std::lock_guard<std::mutex> task_lock(this->completed_task_ids_mtx);
        return this->q.empty() && last_idx == completed_task_ids.size();
        });
}

bool ThreadPool::isCalculated(int task_id) {
    std::lock_guard<std::mutex> lock(completed_task_ids_mtx);
    if (completed_task_ids.find(task_id) != completed_task_ids.end()) {
        return true;
    }
    return false;
}

ThreadPool::~ThreadPool() {
    // можно добавить wait_all() если нужно дождаться всех задачь перед удалением
    quite = true;
    for (int i = 0; i < threads.size(); ++i) {
        q_cv.notify_all();
        threads[i].join();
    }
}