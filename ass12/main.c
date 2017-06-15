/* Program to compute Pi using Monte Carlo methods */
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
#if os(Linux)
return Int(random() % (max + 1))
#else
arc4random_uniform
double x = (double)arc4random_uniform(RAND_MAX) / RAND_MAX;
#endif
*/

#define PRNG_BUFSZ 32 // pseudorandom number generator buffer size

typedef struct data {
    long long count;
    long long iterations;
} Data;

void *calculate(void *arg)
{
	unsigned int seed = time(NULL);

    Data *argument = (Data *)arg;
	printf("iteration %lld\n", argument->iterations);

    long long count = 0;
    for (int i = 0; i < argument->iterations; i++) {
        double x = (double) rand_r(&seed) / RAND_MAX;

        double y = (double) rand_r(&seed) / RAND_MAX;

        double z = x * x + y * y;
        if (z <= 1.0)
            count++;
    }

    argument->count = count;

    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("Usage: ./pi [iterations] [threads]\n");
        return 1;
    }

    long long iterations = atol(argv[1]);
    int threads = atoi(argv[2]);
    printf("Iterations: %d, threads: %d\n", iterations, threads);

	long long orig = iterations;
	long long slice = iterations / threads;
	Data args[threads];
	pthread_t threadPool[threads];
    for (int i = 0; i < threads; i++) {
        args[i].iterations = (i == threads - 1 ? iterations : slice);

        pthread_create(&threadPool[i], NULL, calculate, (void *)&args[i]);

		iterations -= slice;
    }

	for(int i = 0; i < threads; i++) {
        void *ret = NULL;
        pthread_join(threadPool[i], ret);
	}

    long long count = 0;
    for (int i = 0; i < threads; i++) {
        count += args[i].count;
    }

    double pi = (double)count / orig * 4;
    printf("Estimate of pi is %.15f.\nError = %e\n", pi,
           (acos(-1) - pi) / acos(-1));
}
