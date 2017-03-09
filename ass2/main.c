#include <stdio.h>

typedef long long ll;

int main()
{
	ll data[100];
	data[0] = 0;
	data[1] = 1;

	for(int i = 2; i <= 90; i++) {
		data[i] = data[i - 1] + data[i - 2];
	}

	for(int i = 0; i < 90; i++)
		printf("%3d = %18lld\n", i + 1, data[i]);

	return 0;
}
