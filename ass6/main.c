#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>

#define BUF_LEN 100000

// define terminal colorful text output
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define GREEN "\033[0;32;32m"
#define CYAN "\033[0;36m"

typedef struct dirent Dirent;

typedef struct {
    int fd;
    char *pathname;
} MyInotifyData;

int inotifyDataIdx;
int inotifyDataCapacity;
MyInotifyData *inotifyData;

void saveInotifyFileDescriptor(int fd, char *pathname)
{
    if (inotifyData == NULL) {
        inotifyDataCapacity = 1;
        inotifyData =
            (MyInotifyData *)malloc(sizeof(MyInotifyData) * inotifyDataCapacity);
    } else {
        if (inotifyDataIdx >= inotifyDataCapacity) {
            inotifyDataCapacity *= 2;
            MyInotifyData *newData =
                (MyInotifyData *)malloc(sizeof(MyInotifyData) * inotifyDataCapacity);
            memcpy(newData, inotifyData,
                   sizeof(MyInotifyData) * inotifyDataCapacity / 2);
            free(inotifyData);
            inotifyData = newData;
        }
    }

    inotifyData[inotifyDataIdx].fd = fd;
    inotifyData[inotifyDataIdx].pathname =
        (char *)malloc(sizeof(char) * (strlen(pathname) + 2));
    strcpy(inotifyData[inotifyDataIdx].pathname, pathname);
    inotifyDataIdx++;
}

int cmp(const void *a, const void *b)
{
    char *aa = (char *)a;
    char *bb = (char *)b;
    return strcmp(aa, bb);
}

void addInotifyToDirectory(char *pathname)
{
    int fd = inotify_init();
    if (fd == -1) {
        perror("inotify_init() error");
        exit(-1);
    }

    int watch = inotify_add_watch(fd, pathname, IN_ALL_EVENTS);
    if (watch == -1) {
        perror("inotify_add_watch() error");
        exit(-1);
    }

    saveInotifyFileDescriptor(fd, pathname);
}

void listDirectory(char *pathname)
{
    DIR *dp = opendir(pathname);
    if (dp == NULL) {
        perror("opendir() error");
        exit(-1);
    }

    Dirent *dptr = readdir(dp); // obtain directory entry struct
    if (dptr == NULL && errno != 0) {
        perror("readdir() error");
        exit(-1);
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
        char toQuerypathname[512];
        strcpy(toQuerypathname, pathname);
        strcat(toQuerypathname, "/"); // don't forget the /
        strcat(toQuerypathname,
               dptr->d_name); // get filepath of a file under this directory
        // printf("toQuerypathname %s\n", toQuerypathname);

        struct stat buf;
        if (stat(toQuerypathname, &buf) != 0) {
            perror("stat() error");
            exit(-1);
        }
        // int perm = (buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
        // printf("%o\n", perm);
        if (S_ISDIR(buf.st_mode)) {
            // printf(CYAN "This is a directory\n" NONE);

            if (strcmp(dptr->d_name, ".") != 0 && strcmp(dptr->d_name, "..") != 0)
                strcpy(dirPathList[dirPathListIdx++], toQuerypathname);
        }

        dptr = readdir(dp); // go to next file
    }

    // sort file list and print them
    printf(GREEN "%s:\n" NONE, pathname);
    qsort(fileNameList, fileNameListIdx, sizeof(char) * 256, cmp);
    for (int i = 0; i < fileNameListIdx; i++) {
        printf("%s\n", fileNameList[i]); // file name
    }

    // recursively go to directory
    for (int i = 0; i < dirPathListIdx; i++) {
        printf(GREEN "Going to directory %s\n", dirPathList[i]);
        listDirectory(dirPathList[i]);
    }

    closedir(dp);

	addInotifyToDirectory(pathname);
}

void listenForInotifyEvents()
{
    printf("\n\n");
    while (1) {
        for (int i = 0; i < inotifyDataIdx; i++) {
			int fd = inotifyData[i].fd;

            // use polling to avoid read blocking
            struct pollfd pfd = {fd, POLLIN, 0};
            int pollRet = poll(&pfd, 1, 50); // timeout of 50ms
            if (pollRet < 0) {
                perror("poll() error");
				exit(-1);
            } else if (pollRet == 0) {
                // Timeout with no events, move on.
				continue;
            }

            char buf[BUF_LEN];
            int num = read(fd, buf, BUF_LEN);
            if (num < 0) {
                perror("read() error");
                exit(-1);
            }

            if (num == 0) // no event
                continue;

            printf(CYAN "Printing inotify events for %s\n" NONE,
                   inotifyData[i].pathname);
            for (char *p = buf; p < buf + num;) {
                struct inotify_event *event = (struct inotify_event *)p;

                if (event->mask & IN_ACCESS)
                    puts("IN_ACCESS");
                else if (event->mask & IN_ATTRIB)
                    puts("IN_ATTRIB");
                else if (event->mask & IN_CLOSE_WRITE)
                    puts("IN_CLOSE_WRITE");
                else if (event->mask & IN_CLOSE_NOWRITE)
                    puts("IN_CLOSE_NOWRITE");
                else if (event->mask & IN_CREATE)
                    puts("IN_CREATE");
                else if (event->mask & IN_DELETE)
                    puts("IN_DELETE");
                else if (event->mask & IN_DELETE_SELF)
                    puts("IN_DELETE_SELF");
                else if (event->mask & IN_MODIFY)
                    puts("IN_MODIFY");
                else if (event->mask & IN_MOVE_SELF)
                    puts("IN_MOVE_SELF");
                else if (event->mask & IN_MOVED_FROM)
                    puts("IN_MOVED_FROM");
                else if (event->mask & IN_MOVED_TO)
                    puts("IN_MOVED_TO");
                else if (event->mask & IN_OPEN)
                    puts("IN_OPEN");
                else {
                    printf(RED "Unrecognized event\n" NONE);
                    printf(RED "mask = %o\n" NONE, event->mask);
                }

                if (event->len > 0)
                    printf("name = %s\n", event->name);

                p += sizeof(struct inotify_event) + event->len;
            }
            printf(CYAN "End printing inotify data for %s\n\n" NONE,
                   inotifyData[i].pathname);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf(RED "Please supply one directory for this program to work\n" NONE);
        return 1;
    }

    inotifyData = NULL;
    inotifyDataIdx = inotifyDataCapacity = 0;

    printf(GREEN "Will be monitoring directory %s\n\n" NONE, argv[1]);
    listDirectory(argv[1]);

    listenForInotifyEvents();

    return 0;
}
