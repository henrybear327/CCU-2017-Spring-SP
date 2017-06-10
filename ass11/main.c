#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "color.h"

int main(int argc, char **argv)
{
    if (argc == 1) {
        printf(RED "Please provide a filename\n" RESET);
        return 0;
    }

    char *filename = argv[1];

    // create pipe
    int pipefd[2]; // 0 - read, 1 - write
    pipe(pipefd);

    // fork child 1 -> cat filename -> pipe to child 2
    pid_t ret = fork();
    if (ret == 0) {
        // child
        // https://stackoverflow.com/questions/28507950/calling-ls-with-execv

        // printf("In child\n");

        // close stdout and use pipe write instead
        // stdin, stdout, stderr -> 0, 1, 2
        dup2(pipefd[1], 1); // duplicate fd, using the lowest fd avail -> will be 1
        close(pipefd[1]);       // close stdout
        close(pipefd[0]);       // close stdout

        char *arguments[] = {"/bin/cat", filename,
                             NULL
                            };     // need to include the program to exec
        execv(arguments[0], arguments); // remember the backslash!
    }
	close(pipefd[1]); // ?

    // printf("In parent\n");
    int status;
    wait(&status);

    // fork child 2 -> execv wc
    pid_t ret2 = fork();
    if (ret2 == 0) {
        dup2(pipefd[0], 0);
        close(pipefd[0]);

        char *arguments[] = {"wc",
                             NULL
                            };       // need to include the program to exec
        execv("/usr/bin/wc", arguments); // remember the backslash!

        // char buffer[10000];
        // while (fgets(buffer, 10000, stdin) != NULL) {
        // printf("%s\n", buffer);
        // }
    }

    return 0;
}
