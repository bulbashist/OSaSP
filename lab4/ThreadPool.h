#include <queue>
#include <future>
#include <mutex>
#include <condition_variable>
#include <unordered_set>
#include <condition_variable>
#include <atomic>
#include <vector>

using namespace std;

class ThreadPool {
public:
	ThreadPool(int num_threads);
	~ThreadPool();
	template <typename Func, typename ...Args>
	int add_task(Func&& task_func, Args&&... args);
	void wait_all();
	void run();

private:
	//Task queue
	std::queue<std::pair<std::future<void>, int>> queue;

	//Queue mutex and condition variable
	std::mutex q_mtx;
	std::condition_variable q_cv;

	//Idk
	std::unordered_set<int> completed_task_ids;

	//Set mutex and condition variable
	std::condition_variable completed_task_ids_cv;
	std::mutex completed_task_ids_mtx;
	
	//Threads for pool
	std::vector<std::thread> threads;

	//Thread-changing vars
	std::atomic<bool> quite { false };
	std::atomic<int> last_idx = 0;
};

template <typename Func, typename ...Args>
inline int ThreadPool::add_task(Func&& task_func, Args&&... args) {
	//id задачи
	int task_idx = this->last_idx++;
	std::lock_guard<std::mutex> q_lock(q_mtx);
	this->queue.emplace(std::pair(std::async(std::launch::deferred, task_func, args...), task_idx));
	// делаем notify, чтобы проснулся один спящий поток
	this->q_cv.notify_one();
	return task_idx;
}