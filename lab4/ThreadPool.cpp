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

        // ���� ���� ������, �� ���� ������, ����� - ��������
        // ���� �� ����� � ����������, �� quite ����� true � �� �� ����� 
        // ����� ���������� ���� ����� � ������ �� �����
        this->q_cv.wait(lock, [this]()->bool { return !this->q.empty() || quite; });

        if (!q.empty()) {
            auto elem = std::move(q.front());
            q.pop();
            lock.unlock();

            // ��������� ������ ���� std::future (��������� �������) 
            elem.first.get();

            std::lock_guard<std::mutex> lock(completed_task_ids_mtx);

            // ��������� ����� ���������� ������ � ������ �����������
            completed_task_ids.insert(elem.second);

            // ������ notify, ����� ��������� ������
            completed_task_ids_cv.notify_all();
        }
    }
}



void ThreadPool::wait(int task_id) {
    std::unique_lock<std::mutex> lock(completed_task_ids_mtx);

    // ������� ������ notify � ������� run (��������� ����� ���������� ������)
    completed_task_ids_cv.wait(lock, [this, task_id]()->bool {
        return completed_task_ids.find(task_id) != completed_task_ids.end();
        });
}

void ThreadPool::wait_all() {
    std::unique_lock<std::mutex> lock(this->q_mtx);

    // ������� ������ notify � ������� run (��������� ����� ���������� ������)
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
    // ����� �������� wait_all() ���� ����� ��������� ���� ������ ����� ���������
    quite = true;
    for (int i = 0; i < threads.size(); ++i) {
        q_cv.notify_all();
        threads[i].join();
    }
}