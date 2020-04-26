// taskpool.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#ifndef __TASK_POOL_H_
#define __TASK_POOL_H_

#include <vector>
#include <deque>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class CTaskPool {
public:
	CTaskPool(size_t threads)
		: stop(false)
	{
		for (size_t i = 0; i < threads; ++i)
			workers.emplace_back(
				[this]
				{
					for (;;)
					{
						std::function<void()> task;

						{
							std::unique_lock<std::mutex> lock(this->queue_mutex);
							this->condition.wait(lock,
								[this] { return this->stop || !this->tasks.empty(); });
							if (this->stop && this->tasks.empty())
								return;
							task = std::move(this->tasks.front());
							this->tasks.pop_front();
						}

						task();
					}
				}
				);
	}
	~CTaskPool() {
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			stop = true;
		}
		condition.notify_all();
		std::for_each(workers.begin(), workers.end(), [](auto & worker) {
			worker.join();
			});
	}
private:
	// need to keep track of threads so we can join them
	std::vector<std::thread> workers;
	// the task queue
	std::deque<std::function<void()>> tasks;

	// synchronization
	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;

public:
	// add new work item to the pool
	template<class F, class... Args>
	auto enqueue(F&& f, Args&&... args)
		->std::future<typename std::result_of<F(Args...)>::type> {
		using return_type = typename std::result_of<F(Args...)>::type;

		auto _promise = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);

		std::future<return_type> res = _promise->get_future();
		{
			std::unique_lock<std::mutex> lock(queue_mutex);

			// don't allow enqueueing after stopping the pool
			if (stop)
			{
				throw std::runtime_error("enqueue on stopped TaskPool");
			}

			tasks.emplace_back([_promise]() { (*_promise)(); });
		}
		condition.notify_one();
		return res;
	}
};

#endif  //!__TASK_POOL_H_

// TODO: Reference additional headers your program requires here.
