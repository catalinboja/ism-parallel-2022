#include <string>
#include <vector>
#include <stdio.h>
#include <omp.h>
#include "Methods.h"

int main() {

	const long N = 5e5;

	const int noCores = omp_get_num_procs();
	printf("\n Available cores = %d", noCores);

	benchmark("Sequential solution", N, sequentialSolution);
	benchmark("Parallel solution with reace condition", N, parallelRaceSolution);
	benchmark("Parallel solution with a mutex", N, parallelMutexSolution);
}