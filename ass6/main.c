#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define N 100000

typedef struct dirent Dirent;

int main()
{
    char pathName[N];
    getcwd(pathName, N);

    printf("%s\n", pathName);

    DIR *dp = opendir(pathName);
    Dirent *dptr = readdir(dp);
    while (dptr != NULL) {
        printf("%s\n", dptr->d_name);

        dptr = readdir(dp);
    }

    closedir(dp);

    return 0;
}
