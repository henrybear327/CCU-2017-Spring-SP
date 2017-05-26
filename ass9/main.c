#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    pid_t pid;
    pid = getpid();
    printf("%d\n", pid);

    pid = fork();
    if (pid > 0)
        printf("%d\n", pid);

    pid = fork();
    if (pid > 0)
        printf("%d\n", pid);

    pid = fork();
    if (pid > 0)
        printf("%d\n", pid);

    pid = fork();
    if (pid > 0)
        printf("%d\n", pid);

    return 0;
}
