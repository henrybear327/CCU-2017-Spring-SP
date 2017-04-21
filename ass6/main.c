#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unistd.h>
#include <string.h>

#define N 100000

// define terminal colorful text output
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define GREEN "\033[0;32;32m"
#define CYAN "\033[0;36m"

typedef struct dirent Dirent;

int cmp(const void *a, const void *b)
{
	char *aa = (char *)a;
	char *bb = (char *)b;
	return strcmp(aa, bb);
}

void listDirectory(char *pathName)
{
	DIR *dp = opendir(pathName);
	if (dp == NULL) {
		perror("opendir() error");
		return;
	}

	Dirent *dptr = readdir(dp); // obtain directory entry struct
	if (dptr == NULL && errno != 0) {
		perror("readdir() error");
		return;
	}

	// file name list
	int fileNameListIdx = 0;
	char fileNameList[100][256];

	// directory path list 
	int dirPathListIdx = 0;
	char dirPathList[100][512];

	// print out files in the current directory using directory entry struct
	while (dptr != NULL) {
		// printf("%s\n", dptr->d_name); // file name
		strcpy(fileNameList[fileNameListIdx++], dptr->d_name);
		
		// setup info for stat() to use
		char toQueryPathName[512];
		strcpy(toQueryPathName, pathName);
		strcat(toQueryPathName, "/");  // don't forget the /
		strcat(toQueryPathName, dptr->d_name); // get filepath of a file under this directory
		// printf("toQueryPathName %s\n", toQueryPathName);

		struct stat buf;
		if(stat(toQueryPathName, &buf) != 0) {
			perror("stat() error");
			return;
		}
		// int perm = (buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
		// printf("%o\n", perm);
		if (S_ISDIR(buf.st_mode)) {
			// printf(CYAN "This is a directory\n" NONE);

			if(strcmp(dptr->d_name, ".") != 0 && strcmp(dptr->d_name, "..") != 0)
				strcpy(dirPathList[dirPathListIdx++], toQueryPathName);
		}

		dptr = readdir(dp); // go to next file
	}

	// sort file list and print them
	printf(GREEN "%s:\n" NONE, pathName);
	qsort(fileNameList, fileNameListIdx, sizeof(char) * 256, cmp);
	for(int i = 0; i < fileNameListIdx; i++) {
		printf("%s\n", fileNameList[i]); // file name
	}

	// recursively go to directory
	for(int i = 0; i < dirPathListIdx; i++) {
		printf(GREEN "Going to directory %s\n", dirPathList[i]);
		listDirectory(dirPathList[i]);
	}

	closedir(dp);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf(RED "Please supply one directory for this program to work\n" NONE);
		return 1;
	}

	printf(GREEN "Will be monitoring directory %s\n\n" NONE, argv[1]);
	listDirectory(argv[1]);

	return 0;
}
