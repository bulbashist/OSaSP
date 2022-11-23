/*
init(num_threads) - �����, ��������� ������ �� num_threads �������.
� ����� ���������� � �������� ������� ������ ����� ��������� �����������.

add_task(task_func, args) - �� ����������� ����� ���������� ����� ������.
��������� ������� task_func � ��������� ������ ������� args � ���������� task_id(���������� ����� ������).

wait(task_id) - ����������� �����, ��������� ���������� ������ � ��������� task_id.
� ������ ���������� �� �� ����� ��������� ��������� ������ �������(�� �������� ������ ���������� ���� �����),
��� ���� ������� ����������� ������ ���������� void.

wait_all() - ����������� �����, ������������ ���������� ���� �����.

calculated(task_id) - �� ����������� �����, ����������� ���� �� ��������� ������ � ������� task_id.

shutdown() - ����������� �����, ������������ ���������� ���� ����� � ����������� ������ Thread Pool.
��� ����������� ���������� ������ ��������� ����� ������������ ����������(���� ����� ������������� �������� � �����).
*/

/*
// ������� ����� - ������ �������(������), ������� ����� ��������� � ����� ������
std::queue<std::pair<std::future<void>, int64_t>> q;

std::mutex q_mtx;
std::condition_variable q_cv;

// �������� � ������ ��������� ����������� ������
std::unordered_set<int64_t> completed_task_ids;

std::condition_variable completed_task_ids_cv;
std::mutex completed_task_ids_mtx;

std::vector<std::thread> threads;

// ���� ���������� ������ thread_pool
std::atomic<bool> quite{ false };

// ���������� �������� id ������� ����� ����� ��������� ������
std::atomic<int64_t> last_idx = 0;
*/

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
	void wait(int task_id);
	void wait_all();
	bool isCalculated(int task_id);
	void run();

	std::queue<std::pair<std::future<void>, int>> q;
	std::mutex q_mtx;
	std::condition_variable q_cv;
	std::unordered_set<int> completed_task_ids;
	std::condition_variable completed_task_ids_cv;
	std::mutex completed_task_ids_mtx;
	std::vector<std::thread> threads;
	std::atomic<bool> quite{ false };
	std::atomic<int> last_idx = 0;
};

template <typename Func, typename ...Args>
inline int ThreadPool::add_task(Func&& task_func, Args&&... args) {
	// �������� �������� ������� ��� ����� ������
	int task_idx = this->last_idx++;
	std::lock_guard<std::mutex> q_lock(q_mtx);
	this->q.emplace(std::async(std::launch::deferred, task_func, args...), task_idx);
	// ������ notify_one, ����� ��������� ���� ������ ����� (���� ����� ����)
	// � ������ run
	this->q_cv.notify_one();
	return task_idx;
}