// signal.h is a POSIX library, not a standard C library, remove the -std=c99
// flag.
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <unistd.h>
#include <unistd.h>

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int main()
{
    sigset_t mask;
    int sfd;
    struct signalfd_siginfo fdsi;
    ssize_t s;

    printf("pid = %d\n", getpid());

    // sigfillset(&mask);

    // add signals to observe
    /*
    ctrl+c ctrl+\ ?
    SIGINT SIGQUIT
    */
    printf("Add SIGINT to mask\n");
    sigaddset(&mask, SIGINT);
    printf("Add SIGTERM to mask\n");
    sigaddset(&mask, SIGTERM);
    printf("Add SIGQUIT to mask\n");
    sigaddset(&mask, SIGQUIT);
    printf("Add SIGWINCH to mask\n");
    sigaddset(&mask, SIGWINCH);

    /* Block signals so that they aren't handled
       according to their default dispositions */

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        handle_error("sigprocmask");

    sfd = signalfd(-1, &mask, 0);
    if (sfd == -1) // fail to create fd
        handle_error("signalfd");

    for (;;) {
        s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
        if (s != sizeof(struct signalfd_siginfo)) // check if the received data is
            // of correct size (type)
            handle_error("read");

        if (fdsi.ssi_signo == SIGINT) {
            printf("Got SIGINT\n");
        } else if (fdsi.ssi_signo == SIGQUIT) {
            printf("Got SIGQUIT\n");
        } else if (fdsi.ssi_signo == SIGTERM) {
            printf("Got SIGTERM\n");
        } else if (fdsi.ssi_signo == SIGWINCH) {
            printf("Got SIGWINCH\n");
        } else {
            printf("Read unexpected signal %d\n", fdsi.ssi_signo);
        }
    }
}
