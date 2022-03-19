#pragma once
#include <string>
#include <omp.h>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

void countPrimes(long lowerLimit, long upperLimit, long& result) {
	for (long i = lowerLimit; i < upperLimit; ++i) {
		bool isPrime = true;
		for (long j = 2; j < i / 2; ++j) {
			if (i % j == 0) {
				isPrime = false;
				break;
			}
		}
		if (isPrime) {
			result += 1;
		}
	}
}

void syncCountPrimes(long lowerLimit, long upperLimit, long& result, mutex& mutex) {
	for (long i = lowerLimit; i < upperLimit; ++i) {
		bool isPrime = true;
		for (long j = 2; j < i / 2; ++j) {
			if (i % j == 0) {
				isPrime = false;
				break;
			}
		}
		mutex.lock();
		if (isPrime) {
			result += 1;
		}
		mutex.unlock();
	}
}


//
// Count number of primes in a given set of numbers
//
long sequentialSolution(long setSize) {
	long noPrimes = 0;
	countPrimes(0, setSize, noPrimes);
	return noPrimes;

}

//
// Parallel solution with race condition on result
//

long parallelRaceSolution(long setSize) {
	long noPrimes = 0;
	int noThreads = omp_get_num_procs();

	vector<thread> threads;
	long intervalSize = setSize / noThreads;

	for (int i = 0; i < noThreads; ++i) {
		long lowerLimit = i * intervalSize;
		long upperLimit = (i + 1) * intervalSize;
		if (i == noThreads - 1) {
			upperLimit = setSize;
		}
		threads.push_back(thread(countPrimes, lowerLimit, upperLimit, ref(noPrimes)));
	}
	for (int i = 0; i < noThreads; i++) {
		threads[i].join();
	}
	return noPrimes;
}

long parallelMutexSolution(long setSize) {
	long noPrimes = 0;
	int noThreads = omp_get_num_procs();

	mutex lockObject;

	vector<thread> threads;
	long intervalSize = setSize / noThreads;

	for (int i = 0; i < noThreads; ++i) {
		long lowerLimit = i * intervalSize;
		long upperLimit = (i + 1) * intervalSize;
		if (i == noThreads - 1) {
			upperLimit = setSize;
		}
		threads.push_back(thread(syncCountPrimes, lowerLimit, upperLimit, ref(noPrimes), ref(lockObject)));
	}
	for (int i = 0; i < noThreads; i++) {
		threads[i].join();
	}
	return noPrimes;
}

void benchmark(string description, 
	long setSize,
	long (* functionPointer)(long)) {

	printf("\n Test for %s", description.c_str());
	double tStart = omp_get_wtime();

	long result = functionPointer(setSize);

	double tFinal = omp_get_wtime();

	printf("\n Result is %d", result);
	printf("\n Required time = %f seconds", tFinal - tStart);

}