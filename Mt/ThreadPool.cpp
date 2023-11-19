#include "ThreadPool.hpp"

using namespace util::mt;

ThreadPool::ThreadPool(size_t n) {
	threads.reserve(n);
	for (size_t i = 0; i < n; ++i) {
		threads.push_back(std::jthread([this](std::stop_token stop) {
			while (!stop.stop_requested()) {
				PTastArgsPair task;
				if (tasksQueue.popWaitFor(task, std::chrono::milliseconds(1000))) {
					task.first(task.second);
				}
			}
			}));
	}
}
ThreadPool::~ThreadPool() {
	for (size_t i = 0; i < threads.size(); ++i) {
		threads[i].request_stop();
	}
	for (size_t i = 0; i < threads.size(); ++i) {
		if (threads[i].joinable()) threads[i].join();
	}
}