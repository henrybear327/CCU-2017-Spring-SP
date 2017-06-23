#include <assert.h>
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

/*
integrate x^2 from 0 to 8

x^3 / 3
8^3 / 3 = 170
*/

double val(double x)
{
    return x * x;
}

#define TIMES 10000
#define MAX_X (8 * TIMES)
#define MAX_Y (64 * TIMES)

void gen(int *inside)
{
    double x = rand() % MAX_X;
    x /= TIMES;
    double y = rand() % MAX_Y;
    y /= TIMES;

    if (y <= val(x))
        (*inside)++;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf(RED "Please supply [random points] [threads]\n" RESET);
        return 0;
    }

    srand(time(NULL));

    int randomPoints = atoi(argv[1]);
    int threads = atoi(argv[2]);
    int inside = 0;

    printf(CYAN "Arguments: %d %d\n" RESET, randomPoints, threads);

    inside = 0;
    for (int i = 0; i < randomPoints; i++) {
        gen(&inside);
    }

    printf("%.15f%%\n", ((double)inside / (double)randomPoints)); // 33.2%
    printf("%.15f\n", 8.0 * 64.0 * ((double)inside / (double)randomPoints));

    return 0;
}
