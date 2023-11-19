#include "TestThreadPool.hpp"
#include <iostream>

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