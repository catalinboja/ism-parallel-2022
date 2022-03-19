#pragma once
#include <string>
#include <omp.h>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

void syncVerifyPrime(long value, long& result, mutex& mutex) {
	bool isPrime = true;
	for (long j = 2; j < value / 2; ++j) {
		if (value % j == 0) {
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

void ompCountPrimes(long lowerLimit, long upperLimit, long& result) {

	long localResult = 0;

#pragma omp parallel for reduction(+:localResult) schedule(dynamic, 1000)
	for (long i = lowerLimit; i < upperLimit; ++i) {
		bool isPrime = true;
		for (long j = 2; j < i / 2; ++j) {
			if (i % j == 0) {
				isPrime = false;
				break;
			}
		}
		if (isPrime) {
			localResult += 1;
		}
	}

	result = localResult;
}



void syncCountPrimes(long lowerLimit, long upperLimit, long& result, mutex& mutex) {

	double tStart = omp_get_wtime();

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

	double tFinal = omp_get_wtime();
	printf("\n Thread duration  = %f seconds", tFinal - tStart);
}

void syncCountPrimesWithGivenStep(long lowerLimit, long upperLimit, long stepSize, long& result, mutex& mutex) {

	double tStart = omp_get_wtime();

	for (long i = lowerLimit; i < upperLimit; i += stepSize) {
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

	double tFinal = omp_get_wtime();
	printf("\n Thread duration  = %f seconds", tFinal - tStart);
}

void countPrimesWithGivenStep(long lowerLimit, long upperLimit, long stepSize, long& result) {

	double tStart = omp_get_wtime();

	for (long i = lowerLimit; i < upperLimit; i += stepSize) {
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

	double tFinal = omp_get_wtime();
	printf("\n Thread duration  = %f seconds", tFinal - tStart);
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

long parallelLoadBalancingSolution(long setSize) {
	
	long result = 0;
	int noThreads = omp_get_num_procs();
	mutex lockObject;
	vector<thread> threads;

	for (int i = 0; i < noThreads; i++) {
		threads.push_back(
			thread(syncCountPrimesWithGivenStep, i, setSize, noThreads, ref(result), ref(lockObject)));
	}
	for (int i = 0; i < noThreads; i++) {
		threads[i].join();
	}

	return result;
}

long parallelBetterLoadBalancingSolution(long setSize) {

	long result = 0;
	int noThreads = omp_get_num_procs();
	mutex lockObject;
	vector<thread> threads;

	//we exclude the even values - they are not prime numbers
	long startingValue = 1;
	for (int i = 0; i < noThreads; i++) {
		threads.push_back(
			thread(syncCountPrimesWithGivenStep, startingValue, setSize, noThreads*2, ref(result), ref(lockObject)));
		startingValue += 2;
	}
	for (int i = 0; i < noThreads; i++) {
		threads[i].join();
	}

	//add 0, 1 and 2
	result += 3;

	return result;
}

long parallelBetterLoadBalancingSolutionWithoutMutex(long setSize) {

	long result = 0;
	int noThreads = omp_get_num_procs();
	vector<thread> threads;

	long** results = new long* [noThreads];
	for (int i = 0; i < noThreads; i++) {
		results[i] = new long[1];
		results[i][0] = 0;
	}

	//we exclude the even values - they are not prime numbers
	long startingValue = 1;
	for (int i = 0; i < noThreads; i++) {
		threads.push_back(
			thread(countPrimesWithGivenStep, startingValue, setSize, noThreads * 2, ref(results[i][0])));
		startingValue += 2;
	}
	for (int i = 0; i < noThreads; i++) {
		threads[i].join();
	}
	for (int i = 0; i < noThreads; i++) {
		result += results[i][0];
	}

	//add 0, 1 and 2
	result += 3;

	return result;
}

long ompParallelSolution(long setSize) {
	long result = 0;
	ompCountPrimes(0, setSize, result);
	return result;
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