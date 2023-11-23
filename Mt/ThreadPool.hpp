#pragma once
#include "SafeQueue.hpp"
#include <future>
#include <any>
#include <thread>
#include <cassert>

namespace util::mt {

	class Null_t {

	};

	class ThreadPool {
	public:
		using TaskT = std::any(std::any);
		using PTaskT = std::packaged_task<std::any(std::any&)>;
		using PTaskArgsPair = std::pair<PTaskT, std::any>;
		ThreadPool(size_t n = std::jthread::hardware_concurrency());
		~ThreadPool();
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;
		template<typename Ret, typename... Args>
		// !!!  requirement
		std::future<std::any> pushTask(std::function<Ret(Args...)> f, Args&&... args);
		inline size_t size() const { return threads.size(); }
	private:
		SafeQueue<PTaskArgsPair> tasksQueue;
		std::vector<std::jthread> threads;
	};

	template<typename Ret, typename... Args>
	// !!!  requirement
	std::future<std::any> ThreadPool::pushTask(std::function<Ret(Args...)> f, Args&&... args) {
		auto l = [=](std::any& anyArgs) -> std::any {
			std::tuple<Args...> targs = std::any_cast<std::tuple<Args...>>(anyArgs);
			if constexpr (std::is_same_v<Ret, void>) {
				std::apply(f, targs);
				return Null_t();
			}
			else {
				Ret ret = std::apply(f, targs);
				return ret;
			}
		};
		PTaskT ptask(l);
		auto fut = ptask.get_future();
		tasksQueue.push(PTaskArgsPair{ std::move(ptask), std::tuple<Args...>{std::forward<decltype(args)>(args)...} });
		return fut;
	}

	template<typename ThreadT>
	class RollingThreadPool {
	public:
		using TaskT = std::any(std::any);
		using PTaskT = std::packaged_task<std::any(std::any&)>;
		using PTaskArgsPair = std::pair<PTaskT, std::any>;
		RollingThreadPool(size_t n = std::jthread::hardware_concurrency());
		RollingThreadPool(const RollingThreadPool&) = delete;
		RollingThreadPool& operator=(const RollingThreadPool&) = delete;
		RollingThreadPool(RollingThreadPool&&) = delete;
		RollingThreadPool& operator=(RollingThreadPool&&) = delete;
		~RollingThreadPool();
		template<typename Ret, typename... Args>
		// !!!  requirement
		std::future<std::any> pushTask(size_t threadIdx, std::function<Ret(Args...)> f, Args&&... args);
		size_t getId();
		ThreadT& getThreadObj(size_t id);
		inline size_t size() const { return threads.size(); }
	private:
		std::vector<ThreadT> threads;
		std::atomic_uint64_t curThreadIdx = 0;
	};

	template<typename ThreadT>
	RollingThreadPool<ThreadT>::RollingThreadPool(size_t n) {
		threads.reserve(n);
		for (size_t i = 0; i < n; ++i) {
			//std::vector<std::pair<int, int>> tt;
			//tt.emplace_back(1, 1);
			threads.emplace_back(SafeQueue<PTaskArgsPair>());
		}
	}

	template<typename ThreadT>
	RollingThreadPool<ThreadT>::~RollingThreadPool() {
		for (size_t i = 0; i < threads.size(); ++i) {
			threads[i].request_stop();
		}
		for (size_t i = 0; i < threads.size(); ++i) {
			threads[i].join();
		}
	}

	template<typename ThreadT>
	size_t RollingThreadPool<ThreadT>::getId() {
		// !! NOTE: threads.size() is not thread safe if vector's size can be changed
		return (curThreadIdx.fetch_add(1)) % threads.size();
	}

	template<typename ThreadT>
	ThreadT& RollingThreadPool<ThreadT>::getThreadObj(size_t id) {
		return threads[id];
	}

	template<typename ThreadT>
	template<typename Ret, typename... Args>
	// !!!  requirement
	std::future<std::any> RollingThreadPool<ThreadT>::pushTask(size_t threadIdx, std::function<Ret(Args...)> f, Args&&... args) {
		assert(threadIdx < threads.size());
		auto l = [=](std::any& anyArgs) -> std::any {
			std::tuple<Args...> targs = std::any_cast<std::tuple<Args...>>(anyArgs);
			if constexpr (std::is_same_v<Ret, void>) {
				std::apply(f, targs);
				return Null_t();
			}
			else {
				Ret ret = std::apply(f, targs);
				return ret;
			}
		};
		PTaskT ptask(l);
		auto fut = ptask.get_future();
		threads[threadIdx].queue().push(PTaskArgsPair{std::move(ptask), std::tuple<Args...>{std::forward<decltype(args)>(args)...}});
		return fut;
	}

}