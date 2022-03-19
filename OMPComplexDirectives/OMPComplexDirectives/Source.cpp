#include <string>
#include <omp.h>

using namespace std;

int main() {

	const int N = 10000;
	int* array = new int[N];
	for (int i = 0; i < N; i++) {
		array[i] = i + 1;
	}

	//standard solution
	long sum = 0;
	for (int i = 0; i < N; i++) {
		sum += array[i];
	}
	printf("\n The sum is %d", sum);


	//parallel solution with shared sum and race condition
	sum = 0;
#pragma omp parallel 
	{
		for (int i = 0; i < N; i++) {
			sum += array[i];
		}
	}

	printf("\n The sum is %d", sum);

	//parallel solution with a OMP mutex
	// the for is duplicated on all threads
	omp_lock_t mutex;
	omp_init_lock(&mutex);

	sum = 0;
#pragma omp parallel 
	{
		for (int i = 0; i < N; i++) {
			omp_set_lock(&mutex);
			sum += array[i];
			omp_unset_lock(&mutex);
		}
	}
	omp_destroy_lock(&mutex);
	printf("\n The sum is %d", sum);


	//parallel solution with a OMP critical
	// the for is duplicated on all threads
	sum = 0;
#pragma omp parallel 
	{
		for (int i = 0; i < N; i++) {
#pragma omp critical
			sum += array[i];
		}
	}
	printf("\n The sum is %d", sum);

	//parallel with work sharing of the for loop
	sum = 0;

#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < N; i++) {
			omp_set_lock(&mutex);
			sum += array[i];
			omp_unset_lock(&mutex);
		}
	}
	printf("\n The sum is %d", sum);


	//parallel with local variables and reduction
	sum = 0;

#pragma omp parallel for num_threads(4) reduction(+: sum)
	for (int i = 0; i < N; i++) {
		sum += array[i];
	}

	printf("\n The sum is %d", sum);

	//parallel with reduction and a dynamic scheduler
	sum = 0;

#pragma omp parallel for num_threads(4) reduction(+: sum) schedule(dynamic, 100)
	for (int i = 0; i < N; i++) {
		sum += array[i];
	}

	printf("\n The sum is %d", sum);


#pragma omp parallel num_threads(4)
	{

#pragma omp for reduction(+: sum) schedule(dynamic)
		for (int i = 0; i < 40; i++) {
			printf("\n Thread id %d got iteration %d", omp_get_thread_num(), i);
			sum += array[i];
		}
		printf("\n The sum is %d", sum);

	}
}