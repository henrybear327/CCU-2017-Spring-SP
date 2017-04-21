#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#define N 100000

// define terminal colorful text output
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define GREEN "\033[0;32;32m"
#define CYAN "\033[0;36m"

typedef struct dirent Dirent;

void printFileInformation(Dirent *dptr)
{
    printf("%s\n", dptr->d_name);
}

void listDirectory(char* pathName)
{
    printf("%s:\n", pathName);

    DIR *dp = opendir(pathName);
    if(dp == NULL) {
        perror("opendir() error");
        return;
    }

    Dirent *dptr = readdir(dp);
    if(dptr == NULL && errno != 0) {
        perror("readdir() error");
        return;
    }

    // print out files in the current directory
    while (dptr != NULL) {
        printFileInformation(dptr);

        dptr = readdir(dp);
    }

    closedir(dp);
}

int main(int argc, char** argv)
{
    if(argc != 2) {
        printf(RED "Please supply one directory for this program to work\n" NONE);
        return 1;
    }

    printf(GREEN "Will be monitoring directory %s\n\n" NONE, argv[1]);
    listDirectory(argv[1]);

    return 0;
}
