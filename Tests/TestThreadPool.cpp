#include "TestThreadPool.hpp"
#include <iostream>
#include <span>
#include <cmath>
#include <shared_mutex>
#include <functional>
#include <format>
#include <optional>

using namespace util::mt;
using namespace util::mt::tests;
using namespace std;

class A {
public:
	int plus(int a) {
		i += a;
		return i;
	}
private:
	int i = 10;
};

void printNeko() {
	cout << "neko" << endl;
}

double dd(double&& d) {
	return d * 2;
}

double power2(double d) {
	return pow(d, 2);
}

void util::mt::tests::testThreadPool() {
	double d = 10.0;
	ThreadPool tp;
	auto fut = tp.pushTask(std::function{ power2 }, std::move(d));
	auto fut2 = tp.pushTask(std::function([](int a, int b) { return a + b; }), 2, 3);
	A a;
	auto fut3 = tp.pushTask(std::function<int(A*, int)>(std::mem_fn(&A::plus)), &a, 5);
	auto fut4 = tp.pushTask(std::function{ printNeko });
	//auto fut2 = tp.pushTask(std::)
	double val = std::any_cast<double>(fut.get());
	cout << val << endl;
	int val2 = std::any_cast<int>(fut2.get());
	cout << val2 << endl;
	int val3 = std::any_cast<int>(fut3.get());
	cout << val3 << endl;
	fut4.wait();
	//auto fut = tp.pushTask([](std::any))
}

/*class Observer {
public:
	virtual void removeFd(int fd) = 0;
};

class Observable {
public:
	void setObserver(Observer* obs) {
		observer = obs;
	}
	void notifyRemoveFd(int fd) {
		assert(observer);
		observer->removeFd(fd);
	}
private:
	Observer* observer;
};


class SocketDataHandler : public Observable {
public:
	using TaskT = std::pair<std::packaged_task<std::any(std::any&)>, std::any>;
	using QueueT = SafeQueue<TaskT>;
	SocketDataHandler(QueueT&& tasksQueue)
		: tasksQueue{ std::move(tasksQueue) }
	{
		run();
	}
	SocketDataHandler(const SocketDataHandler&) = delete;
	SocketDataHandler& operator=(const SocketDataHandler&) = delete;
	SocketDataHandler(SocketDataHandler&& other) noexcept {
		std::lock_guard<std::mutex> lck(other.mtx);
		tasksQueue = std::move(other.tasksQueue);
		thread = std::move(other.thread);
		sockets = std::move(other.sockets);
	}
	SocketDataHandler& operator=(SocketDataHandler&& other) noexcept {
		std::lock_guard<std::mutex> lck(other.mtx);
		tasksQueue = std::move(other.tasksQueue);
		thread = std::move(other.thread);
		sockets = std::move(other.sockets);
		return *this;
	}
	~SocketDataHandler() {
		request_stop();
		join();
	}
	void request_stop() {
		thread.request_stop();
	}
	void join() {
		if (thread.joinable()) {
			thread.join();
		}
	}
	QueueT& queue() {
		return tasksQueue;
	}

	void onRead(int epollFd, int socketFd) {
		cout << format("Reading from epoll {} and socket {}\n", epollFd, socketFd);
	}
	void onClose(int epollFd, int socketFd) {
		cout << "Closing...\n";
		notifyRemoveFd(socketFd);
	}
	void run() {
		thread = std::move(std::jthread([this](std::stop_token stop) {
			while (!stop.stop_requested()) {
				TaskT task;
				if (tasksQueue.popWaitFor(task, std::chrono::milliseconds(1000))) {
					task.first(task.second);
				}
			}
			}));
	}

private:
	QueueT tasksQueue;
	std::jthread thread;
	std::vector<std::pair<int, std::vector<char>>> sockets;
	//int epollFd;
	std::mutex mtx;
};

class SocketThreadMapper : public Observer {
public:
	std::optional<size_t> findThreadId(int fd) {
		std::shared_lock<std::shared_mutex> lck(mtx);
		if (auto iter = map.find(fd); iter == map.end()) {
			return std::nullopt;
		}
		else {
			return iter->second;
		}
	}
	void addThreadId(int fd, size_t threadIdx) {
		std::unique_lock<std::shared_mutex> lck(mtx);
		map[fd] = threadIdx;
	}
	void removeFd(int fd) override {
		std::unique_lock<std::shared_mutex> lck(mtx);
		map.erase(fd);
	}
private:
	std::shared_mutex mtx;
	std::unordered_map<int, size_t> map;
};

void util::mt::tests::testRollingThreadPool() {
	SocketThreadMapper mapper;
	RollingThreadPool<SocketDataHandler> rtp;
	for (size_t i = 0; i < rtp.size(); ++i) {
		rtp.getThreadObj(i).setObserver(&mapper);
	}
	int epollFd = 0;
	int sockFd = 1;
	auto id = rtp.getIdx();
	auto& obj = rtp.getThreadObj(id);
	rtp.pushTask(rtp.getIdx(), std::function([&obj](int epollFd, int socketFd) { obj.onRead(epollFd, socketFd); return 0; }), std::move(epollFd), std::move(sockFd));
}*/