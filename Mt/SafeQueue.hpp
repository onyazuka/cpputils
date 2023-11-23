#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

namespace util::mt {
	template<typename T>
	class SafeQueue {
	public:
		SafeQueue() = default;
		SafeQueue(const SafeQueue& other) = delete;
		SafeQueue& operator=(const SafeQueue& other) = delete;
		SafeQueue(SafeQueue&& other) noexcept {
			std::lock_guard<std::mutex> lck2(other.mtx);
			que = std::move(other.que);
		}
		SafeQueue& operator=(SafeQueue&& other) noexcept {
			std::lock_guard<std::mutex> lck2(other.mtx);
			que = std::move(other.que);
			return *this;
		}
		void push(T t) {
			std::lock_guard<std::mutex> lck{ mtx };
			que.push(std::move(t));
			cv.notify_one();
		}
		void popWait(T& t) {
			std::unique_lock<std::mutex> lck{ mtx };
			cv.wait(lck, [this]() {
				return !que.empty();
				});
			t = std::move(que.front());
			que.pop();
		}
		bool popWaitFor(T& t, std::chrono::milliseconds dur) {
			std::unique_lock<std::mutex> lck{ mtx };
			bool hasVal = cv.wait_for(lck, dur, [this]() {
				return !que.empty();
				});
			if (!hasVal) return false;
			t = std::move(que.front());
			que.pop();
			return true;
		}
		bool popTry(T& t) {
			std::lock_guard<std::mutex> lck{ mtx };
			if (que.empty()) {
				return false;
			}
			t = std::move(que.front());
			que.pop();
			return true;
		}
		bool empty() const {
			std::lock_guard<std::mutex> lck{ mtx };
			return que.empty();
		}
		bool size() const {
			std::lock_guard<std::mutex> lck{ mtx };
			return que.size();
		}
		// returns COPY
		T front() const {
			// unique_lock because condition variables can be used with unique lock only (could also use condition_variable_any with shared_lock, but not sure, what is better)
			std::lock_guard<std::mutex> lck{ mtx };
			return que.front();
		}
		T back() const {
			std::lock_guard<std::mutex> lck{ mtx };
			return que.back();
		}
	private:
		std::queue<T> que;
		mutable std::mutex mtx;
		std::condition_variable cv;
	};
}