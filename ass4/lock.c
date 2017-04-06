#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/file.h>

const int MAX_N = 10000;

// define terminal colorful text output
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define GREEN "\033[0;32;32m"
#define CYAN "\033[0;36m"

#define DEBUG 1

int main()
{
	int outputFilefd = open("grade", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR| S_IWUSR);
	if(outputFilefd == -1) {
		perror("output file open() error");
		return EXIT_FAILURE;
	}

	// acquire lock
	int acquireRet = flock(outputFilefd, LOCK_EX);
	if(acquireRet == -1) {
		perror("acquire lock flock() error");
		return EXIT_FAILURE;
	}

	/*
	// release lock
	int releaseRet = flock(outputFilefd, LOCK_UN);
	if(releaseRet == -1) {
	perror("release lock flock() error");
	return EXIT_FAILURE;
	}
	 */

	if(close(outputFilefd)) {
		perror("close() error");
		return EXIT_FAILURE;
	}	

	return 0;
}
