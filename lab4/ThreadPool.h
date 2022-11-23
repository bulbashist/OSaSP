/*
init(num_threads) - метод, создающий массив из num_threads потоков.
В нашей реализации в качестве данного метода будет выступать конструктор.

add_task(task_func, args) - не блокирующий метод добавления новой задачи.
Принимает функцию task_func и аргументы данной функции args и возвращает task_id(уникальный номер задачи).

wait(task_id) - блокирующий метод, ожидающий выполнения задачи с указанным task_id.
В данной реализации мы не будем сохранять результат работы функции(мы исправим данный недостаток чуть позже),
при этом функция обязательно должна возвращать void.

wait_all() - блокирующий метод, дожидающийся завершения всех задач.

calculated(task_id) - не блокирующий метод, проверяющий была ли выполнена задача с номером task_id.

shutdown() - блокирующий метод, дожидающийся завершения всех задач и завершающий работу Thread Pool.
Для корректного завершения работы программы будем использовать деструктор(хотя можно дополнительно добавить и метод).
*/

/*
// очередь задач - хранит функцию(задачу), которую нужно исполнить и номер задачи
std::queue<std::pair<std::future<void>, int64_t>> q;

std::mutex q_mtx;
std::condition_variable q_cv;

// помещаем в данный контейнер исполненные задачи
std::unordered_set<int64_t> completed_task_ids;

std::condition_variable completed_task_ids_cv;
std::mutex completed_task_ids_mtx;

std::vector<std::thread> threads;

// флаг завершения работы thread_pool
std::atomic<bool> quite{ false };

// переменная хранящая id который будет выдан следующей задаче
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
	// получаем значение индекса для новой задачи
	int task_idx = this->last_idx++;
	std::lock_guard<std::mutex> q_lock(q_mtx);
	this->q.emplace(std::async(std::launch::deferred, task_func, args...), task_idx);
	// делаем notify_one, чтобы проснулся один спящий поток (если такой есть)
	// в методе run
	this->q_cv.notify_one();
	return task_idx;
}