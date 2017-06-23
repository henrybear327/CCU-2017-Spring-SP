#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"
#define RESET "\x1B[0m"

typedef long long ll;

#define TIMES 10000
#define MAX_X (8 * TIMES)
#define MAX_Y (64 * TIMES)

typedef struct data {
    int index;
    int inside;
    ll iterations;
} Data;

double val(double x)
{
    return x * x;
}

void *calculate(void *arg)
{
    Data *data = (Data *)arg;
    unsigned int seed = time(NULL);
    printf(GREEN "Thread %d starts with %lld iterations\n" RESET, data->index,
           data->iterations);

    for (ll i = 0; i < data->iterations; i++) {
        int r = rand_r(&seed);
        double x = r % MAX_X;
        x /= TIMES;
        double y = r % MAX_Y;
        y /= TIMES;

        if (y <= val(x))
            (data->inside)++;
    }

    printf(GREEN "Thread %d ends\n" RESET, data->index);
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf(RED "Please supply [random points] [threads]\n" RESET);
        return 0;
    }

    ll randomPoints = atol(argv[1]);
    int threads = atoi(argv[2]);
    printf(CYAN "Arguments: %lld %d\n" RESET, randomPoints, threads);

    Data args[threads];
    pthread_t threadPool[threads];
    ll iterations = randomPoints;
    for (ll i = 0; i < threads; i++) {
        assert(iterations != 0);

        args[i].index = i;
        args[i].inside = 0;
        if (i == threads - 1) {
            args[i].iterations = iterations;
            iterations = 0;
        } else {
            args[i].iterations = randomPoints / threads;
            iterations -= randomPoints / threads;
        }

        pthread_create(&threadPool[i], NULL, calculate, (void *)&args[i]);
    }

    for (int i = 0; i < threads; i++) {
        void *ret = NULL;
        pthread_join(threadPool[i], ret);

        printf(GREEN "Thread %d joined\n" RESET, i);
    }

    int total = 0;
    for (int i = 0; i < threads; i++) {
        total += args[i].inside;
    }

    printf("%.15f%%\n", ((double)total / (double)randomPoints)); // 33.2%
    printf("%.15f\n", 8.0 * 64.0 * ((double)total / (double)randomPoints));

    return 0;
}
