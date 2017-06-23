#include <stdio.h>
#include <stdlib.h>

void throwDart(double *x, double *y)
{
    double min = 0.0, max = 10.0;

    *x = (max - min) * ((double)rand() / (RAND_MAX)) + min; //don't nec. need min - example format
    *y = (max - min) * ((double)rand() / (RAND_MAX)) + min; //^same
}

double Fun(double x)
{
    return ((0.3*x*x) - (0.2*x) - 5); //estimate value of definite integral using this function
}

int main()
{
    double x, y, P; //(x,y) is location, P is number of darts that hit below the curve/total thrown
    int N, e; //N is number of darts, e is area under curve
    int c = 0; //initialize count of darts that hit below the curve

    throwDart(&x, &y);

    printf("How many darts would you like to throw?\n");
    scanf("%d", &N);

    for (int i = 0; i < N; i++)
    {
        if (y <= Fun(x))
        c++;
    }

    P = c/N;
    e = 100.00 * P;

    //most of the following prints just to see what is and isn't working
    printf("X is %lf\n", x);
    printf("Y is %lf\n", y);
    printf("N is %d\n", N);
    printf("c is %d\n", c);
    printf("P is %d\n", P);
    printf("Area under the curve is %d\n", e);

    system("Pause");

}
