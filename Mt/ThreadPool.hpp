#pragma once
#include "SafeQueue.hpp"
#include <future>
#include <any>
#include <thread>

namespace util::mt {

	class Null_t {

	};

	class ThreadPool {
	public:
		using TaskT = std::any(std::any);
		using PTaskT = std::packaged_task<std::any(std::any&)>;
		using PTastArgsPair = std::pair<PTaskT, std::any>;
		ThreadPool(size_t n = std::jthread::hardware_concurrency());
		~ThreadPool();
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;
		template<typename Ret, typename... Args>
		// !!!  requirement
		std::future<std::any> pushTask(std::function<Ret(Args...)> f, Args&&... args);
	private:
		SafeQueue<PTastArgsPair> tasksQueue;
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
		tasksQueue.push(PTastArgsPair{ std::move(ptask), std::tuple<Args...>{std::forward<decltype(args)>(args)...} });
		return fut;
	}

}