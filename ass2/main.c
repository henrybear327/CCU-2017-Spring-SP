#include <stdio.h>

typedef long long ll;

int main()
{
    ll data[100];
    // base case
    data[0] = 0;
    data[1] = 1;

    // use formula f[i] = f[i - 1] + f[i - 2] to compute the ith fibonacci number
    for (int i = 2; i <= 90; i++) {
        data[i] = data[i - 1] + data[i - 2];
    }

    // print the results out on the screen
    for (int i = 0; i <= 90; i++) {
        // align the output for better result displaying
        printf("%3d = %18lld\n", i, data[i]);
    }

    return 0;
}
