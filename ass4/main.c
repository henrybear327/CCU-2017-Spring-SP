#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

const int MAX_N = 10000;

// define terminal colorful text output
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define GREEN "\033[0;32;32m"
#define CYAN "\033[0;36m"

#define DEBUG 1

int main()
{
	char inp[MAX_N];
	while(fgets(inp, MAX_N, stdin) != NULL) {
		// read input
#if DEBUG == 1
		printf(RED "%s" NONE, inp);
#endif

		char *data[6];
		char *token = strtok(inp, " \n\r");

		int i = 0;
		while(token != NULL) {
			data[i] = token;
			token = strtok(NULL, " \n\r");
			i++;
		}

#if DEBUG == 1
		assert(i == 6);
		for(int i = 0; i < 6; i++) 
			printf(RED "%s%c" NONE, data[i], i == 5 ? '\n' : ' ');
#endif
		// make a string
		char toWrite[MAX_N];
		memset(toWrite, 0, sizeof(toWrite));
		sprintf(toWrite, "%s %s %s %s %s %s\n", data[0], data[1], data[2], data[3], data[4], data[5]);
#if DEBUG == 1
		printf(RED "%s" NONE, toWrite);
#endif

		// write to file
		int outputFile = open("grade", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR| S_IWUSR);
		if(outputFile == -1) {
			perror("output file open() error");
			return EXIT_FAILURE;
		}

		int writingByte = write(outputFile, toWrite, sizeof(toWrite));
		if(writingByte == -1) {
			perror("write() error");
			return EXIT_FAILURE;
		}
	}

	return 0;
}
