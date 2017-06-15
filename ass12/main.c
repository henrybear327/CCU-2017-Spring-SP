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
    int count;
    int iterations;
    struct random_data *rand_state;
    char *rand_statebuf;
} Data;

void *calculate(void *arg)
{
    Data *argument = (Data *)arg;

    initstate_r(random(), argument->rand_statebuf, PRNG_BUFSZ,
                argument->rand_state);

    int count = 0;
    for (int i = 0; i < argument->iterations; i++) {
        int res;

        random_r(argument->rand_state, &res);
        double x = (double)res / RAND_MAX;

        random_r(argument->rand_state, &res);
        double y = (double)res / RAND_MAX;

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

    int iterations = atoi(argv[1]);
    int threads = atoi(argv[2]);
    printf("Iterations: %d, threads: %d\n", iterations, threads);

    struct random_data *rand_states =
        (struct random_data *)calloc(threads, sizeof(struct random_data));
    char *rand_statebufs = (char *)calloc(threads, PRNG_BUFSZ);

    pthread_t threadPool[threads];
    Data args[threads];
    for (int i = 0; i < threads; i++) {
        args[i].iterations = iterations;
        args[i].count = 0;
        args[i].rand_state = &rand_states[i];
        args[i].rand_statebuf = &rand_statebufs[i];

        pthread_create(&threadPool[i], NULL, calculate, (void *)&args[i]);

        void *ret = NULL;
        pthread_join(threadPool[i], ret);
    }

    int count = 0;
    for (int i = 0; i < threads; i++) {
        count += args[i].count;
    }

    double pi = (double)count / (iterations * threads) * 4;
    printf("Estimate of pi is %.15f.\nError = %e\n", pi,
           (acos(-1) - pi) / acos(-1));
}
