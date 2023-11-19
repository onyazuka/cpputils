#include "TestSafeQueue.hpp"
#include <random>
#include <format>
#include <iostream>
#include <syncstream>

using namespace util::mt;
using namespace std;

void util::mt::tests::testSafeQueue() {
	SafeQueue<int> sq;
	std::vector<std::jthread> threads;
	size_t N = std::jthread::hardware_concurrency();

	for (size_t i = 0; i < N; ++i) {
		threads.push_back(std::jthread([&sq](std::stop_token stop) {
			std::default_random_engine eng;
			eng.seed(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
			std::uniform_int_distribution distr;
			auto threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
			std::osyncstream oss{ std::cout };
			oss << format("{}: works\n", threadId) << flush_emit;
			while (!stop.stop_requested()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(distr(eng) % 100));
				int action = distr(eng) % 3;
				switch (action) {
				case 0:
				{
					int val = distr(eng) % 1000;
					sq.push(val);
					oss << format("{} push: pushed val {}\n", threadId, val) << flush_emit;
					break;
				}
				case 1:
				{
					int val{};
					bool ok = sq.popTry(val);
					if (ok) {
						oss << format("{} tryPop: popped val {}\n", threadId, val) << flush_emit;
					}
					else {
						oss << format("{} tryPop: not popped\n", threadId) << flush_emit;
					}
					break;
				}
				case 2:
				{
					int val{};
					sq.popWaitFor(val, std::chrono::milliseconds(100));
					oss << format("{} popWait: popped val {}\n", threadId, val) << flush_emit;
					break;
				}
				}

			}
			oss << format("{}: stops\n", threadId) << flush_emit;
			}));
	}
	std::this_thread::sleep_for(std::chrono::seconds(10));
	for (auto& th : threads) {
		th.request_stop();
	}
	for (auto& th : threads) {
		if (th.joinable()) th.join();
	}
}