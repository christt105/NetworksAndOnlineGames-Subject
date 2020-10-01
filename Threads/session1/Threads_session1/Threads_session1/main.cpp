#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <thread>
#include <mutex>

long long int global = 0;
std::mutex mtx;

void function() {
	for (long long int i = 0; i < 1000000; ++i) {
		std::unique_lock<std::mutex> lock(mtx);
		++global;
	}
}

int main() {

	std::thread t[2] = { std::thread(function),std::thread(function) };

	t[0].join();
	t[1].join();

	std::cout << "variable global is:" << global << std::endl;

	system("pause");
	return 0;
}