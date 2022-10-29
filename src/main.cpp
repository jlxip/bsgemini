#include <common.hpp>
#include <iostream>

#define MAX_EVENTS 32

int main() {
	prepareMIMEs();
	start();

	for(size_t i=0; i<(nthreads - 1); ++i) {
		pthread_t thread;
		pthread_create(&thread, NULL, worker, nullptr);
	}

	std::cout << "Spawned " << nthreads << " threads" << std::endl;
	worker(nullptr);
}
