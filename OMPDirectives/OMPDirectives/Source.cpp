#include <string>
#include <omp.h>

int main() {

	printf("\n OMP examples");

	printf("\n Hello from the main thread. Its id is %d", omp_get_thread_num());

	int noCores = omp_get_num_procs();
	printf("\n Number of cores = %d", noCores);

#pragma omp parallel 
	{
		printf("\n Hello from thread %d", omp_get_thread_num());
		printf("\n How are you ?");
	}

	printf("\n -------------------------------");

	int requiredNoThreads = 4;
	omp_set_num_threads(4);

#pragma omp parallel 
	{
		printf("\n Hello again from thread %d", omp_get_thread_num());
		printf("\n How are you ?");
	}

	printf("\n -------------------------------");

	int specialNoThreads = 2;

#pragma omp parallel num_threads(specialNoThreads)
	{
		printf("\n Bye from thread %d", omp_get_thread_num());
	}

	printf("\n -------------------------------");

	int hugeNumberOfThreads = 1e5;

#pragma omp parallel num_threads(hugeNumberOfThreads)
	{
		if (omp_get_thread_num() == 0) {
			int noRealThreads = omp_get_num_threads();
			printf("\n The number of threads in the parallel section is %d",
				noRealThreads);
		}

#pragma omp master 
		{
			int noRealThreads = omp_get_num_threads();
			printf("\n The number of threads in the parallel section is %d",
				noRealThreads);
		}
	}


	//controlling the execution in parallel or not
	bool doItInParallel = true;

	if (doItInParallel) {
#pragma omp parallel 
		{
			//parallel
		}
	}
	else {
		//sequentially
	}

	printf("\n -------------------------------");

	if (noCores < 16)
		doItInParallel = false;

#pragma omp parallel if(doItInParallel)
	{
		printf("\n Hello from a parallel section");
	}

	//managing variables
	//all external varaibles are shared by all threads -> ! race conditions

	printf("\n -------------------------------");


	int counter = 0;
	int noIterations = 1000;

#pragma omp parallel num_threads(4)
	{
		for (int i = 0; i < noIterations; i++) {
			counter += 1;
		}
		printf("\n Counter value is %d", counter);
	}

	printf("\n Final counter value is %d", counter);


	printf("\n -------------------------------");


#pragma omp parallel num_threads(4)
	{
		int localCounter = 0;
		for (int i = 0; i < noIterations; i++) {
			localCounter += 1;
		}
		printf("\n Local counter value is %d", localCounter);
	}

	printf("\n -------------------------------");


#pragma omp parallel num_threads(4) private(counter)
	{
		counter = 0;
		for (int i = 0; i < noIterations; i++) {
			counter += 1;
		}
		printf("\n Counter value is %d", counter);
	}

	printf("\n -------------------------------");

	counter = 500;

#pragma omp parallel num_threads(4) firstprivate(counter) shared(noIterations)
	{
		for (int i = 0; i < noIterations; i++) {
			counter += 1;
		}
		printf("\n Counter value is %d", counter);
	}

	printf("\n The main counter value is %d", counter);


	printf("\n End of example");
}