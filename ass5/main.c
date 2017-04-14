#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// define terminal colorful text output
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define GREEN "\033[0;32;32m"
#define CYAN "\033[0;36m"

#define BUFFER_SIZE 10000

int main(int argc, char** argv)
{
	if(argc != 3) {
		printf(RED "Invalid arguments: correct format is...\nacp file1 file2\n" NONE);
		return EXIT_FAILURE;
	}

	// open file from designated location
	printf(GREEN "Opening file1....\n" NONE);
	int file1fd = open(argv[1], O_WRONLY | O_CREAT | O_APPEND, S_IRUSR| S_IWUSR);
	if(file1fd == -1) {
		perror("output file open() error");
		return EXIT_FAILURE;
	}

	// copy to tmp file	
	printf(GREEN "Opening tmp file....\n" NONE);
	int tmpfd = open(argv[1], O_WRONLY | O_CREAT | O_APPEND, S_IRUSR| S_IWUSR);
	if(tmpfd == -1) {
		perror("output file open() error");
		return EXIT_FAILURE;
	}

	char buffer[BUFFER_SIZE];
	while(1) {
		int readRet = read(file1fd, buffer, BUFFER_SIZE);
		if(readRet == -1) {
			perror("read() error");
			return EXIT_FAILURE;
		}

		int writeRet = write(tmpfd, buffer, readRet);
		if(writeRet == -1) {
			perror("read() error");
			return EXIT_FAILURE;
		}
	}


	// press to continue

	// move tmp file to designated location

	// close
	close(file1fd);
	close(tmpfd);

	return 0;
}
