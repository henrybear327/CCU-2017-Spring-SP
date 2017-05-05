#include <dirent.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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

typedef struct {
    char *directoryName;
} MyDirectoryData;

int directoryDataIdx;
int directoryDataCapacity;
MyDirectoryData *directoryData;

int queueDataIdx;
int queueDataCapacity;
MyDirectoryData *queueData;

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

void saveDirectoryArgument(char *pathname)
{
    if (directoryData == NULL) {
        directoryDataCapacity = 1;
        directoryData = (MyDirectoryData *)malloc(sizeof(MyDirectoryData) *
                        directoryDataCapacity);
    } else {
        if (directoryDataIdx >= directoryDataCapacity) {
            directoryDataCapacity *= 2;
            MyDirectoryData *newData = (MyDirectoryData *)malloc(
                                           sizeof(MyDirectoryData) * directoryDataCapacity);
            memcpy(newData, directoryData,
                   sizeof(MyDirectoryData) * directoryDataCapacity / 2);
            free(directoryData);
            directoryData = newData;
        }
    }

    directoryData[directoryDataIdx].directoryName =
        (char *)malloc(sizeof(char) * (strlen(pathname) + 2));
    strcpy(directoryData[directoryDataIdx].directoryName, pathname);
    directoryDataIdx++;
}

void addDirectoryToQueue(char *pathname)
{
    if (queueData == NULL) {
        queueDataCapacity = 1;
        queueData =
            (MyDirectoryData *)malloc(sizeof(MyDirectoryData) * queueDataCapacity);
    } else {
        if (queueDataIdx >= queueDataCapacity) {
            queueDataCapacity *= 2;
            MyDirectoryData *newData = (MyDirectoryData *)malloc(
                                           sizeof(MyDirectoryData) * queueDataCapacity);
            memcpy(newData, queueData,
                   sizeof(MyDirectoryData) * queueDataCapacity / 2);
            free(queueData);
            queueData = newData;
        }
    }

    queueData[queueDataIdx].directoryName =
        (char *)malloc(sizeof(char) * (strlen(pathname) + 2));
    strcpy(queueData[queueDataIdx].directoryName, pathname);
    queueDataIdx++;
}

int addInotifyToPath(char *pathname, int topLevelfd)
{
    // printf(GREEN "Listen for path = %s\n" NONE, pathname);

    // printf("in %s %d\n", pathname, topLevelfd);

    if (topLevelfd == -1) {
        // create a new inotify instance
        int fd = inotify_init();
        if (fd == -1) {
            perror("inotify_init() error");
            exit(-1);
        }
        saveInotifyFileDescriptor(fd, pathname);

        topLevelfd = fd;
        // printf("in1 %s %d\n", pathname, topLevelfd);
    }

    // attach
    int watch = inotify_add_watch(topLevelfd, pathname, IN_ALL_EVENTS);
    if (watch == -1) {
        perror("inotify_add_watch() error");
        exit(-1);
    }
    // printf("out %s %d\n", pathname, topLevelfd);

    return topLevelfd;
}

int isDirectory(char *pathname)
{
    struct stat buf;
    if (stat(pathname, &buf) != 0) {
        perror("stat() error");
        exit(-1);
    }

    if (S_ISDIR(buf.st_mode))
        return 1;
    return 0;
}

void search(char *pathname, int isTopLevel)
{
    if (isTopLevel == 1) {
        // one first invokation of recursive search, check if this is a directory
        if (isDirectory(pathname) == 1) {
            // add the argument name into list, and keep on searching!
            // printf(CYAN "Argument is a folder!\n" NONE);
            saveDirectoryArgument(pathname);
        } else {
            // file, add inotify directly, and return
            // printf(CYAN "Argument is a file!\n" NONE);
            addDirectoryToQueue(pathname);
            return;
        }
    }
    isTopLevel = 0;
    addDirectoryToQueue(pathname);

    // printf(CYAN "Now searching under path %s\n" NONE, pathname);
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

    // directory path list
    int dirPathListIdx = 0;
    char dirPathList[100][512];

    // print out files in the current directory using directory entry struct
    while (dptr != NULL) {
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

    // recursively go to directory
    for (int i = 0; i < dirPathListIdx; i++) {
        // printf(GREEN "Going to directory %s\n", dirPathList[i]);
        search(dirPathList[i], isTopLevel);
    }

    closedir(dp);
}

void listenForInotifyEvents()
{
    printf("DIR:");
    for (int i = 0; i < directoryDataIdx; i++) {
        printf(" %s", directoryData[i].directoryName);
    }
    printf("\n");

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

            for (char *p = buf; p < buf + num;) {
                // [ch06] {IN_OPEN, IN_ISDIR, }cookie=0 name = null
                printf("[%s] {", inotifyData[i].pathname);

                struct inotify_event *event = (struct inotify_event *)p;

                if (event->mask & IN_ACCESS)
                    printf("IN_ACCESS, ");
                if (event->mask & IN_ATTRIB)
                    printf("IN_ATTRIB, ");
                if (event->mask & IN_CLOSE_WRITE)
                    printf("IN_CLOSE_WRITE, ");
                if (event->mask & IN_CLOSE_NOWRITE)
                    printf("IN_CLOSE_NOWRITE, ");
                if (event->mask & IN_CREATE)
                    printf("IN_CREATE, ");
                if (event->mask & IN_DELETE)
                    printf("IN_DELETE, ");
                if (event->mask & IN_DELETE_SELF)
                    printf("IN_DELETE_SELF, ");
                if (event->mask & IN_MODIFY)
                    printf("IN_MODIFY, ");
                if (event->mask & IN_MOVE_SELF)
                    printf("IN_MOVE_SELF, ");
                if (event->mask & IN_MOVED_FROM)
                    printf("IN_MOVED_FROM, ");
                if (event->mask & IN_MOVED_TO)
                    printf("IN_MOVED_TO, ");
                if (event->mask & IN_OPEN)
                    printf("IN_OPEN, "); // here
                if (event->mask & IN_MOVE)
                    printf("IN_MOVE, ");
                if (event->mask & IN_CLOSE)
                    printf("IN_CLOSE, ");
                if (event->mask & IN_DONT_FOLLOW)
                    printf("IN_DONT_FOLLOW, ");
                if (event->mask & IN_EXCL_UNLINK)
                    printf("IN_EXCL_UNLINK, ");
                if (event->mask & IN_MASK_ADD)
                    printf("IN_MASK_ADD, ");
                if (event->mask & IN_ONESHOT)
                    printf("IN_ONESHOT, ");
                if (event->mask & IN_ONLYDIR)
                    printf("IN_ONLYDIR, ");
                if (event->mask & IN_IGNORED)
                    printf("IN_IGNORED, ");
                if (event->mask & IN_ISDIR)
                    printf("IN_ISDIR, ");
                if (event->mask & IN_Q_OVERFLOW)
                    printf("IN_Q_OVERFLOW, ");
                if (event->mask & IN_UNMOUNT)
                    printf("IN_UNMOUNT, ");

                printf("}cookie=%d name = ", event->cookie);
                if (event->len > 0)
                    printf("%s\n", event->name);
                else
                    printf("NULL\n");

                p += sizeof(struct inotify_event) + event->len;
                fflush(stdout);
            }
        }
    }
}

void attachListener()
{
    int fd = -1;
    for (int i = 0; i < queueDataIdx; i++) {
        char *pathname = queueData[i].directoryName;
        if (fd == -1)
            fd = addInotifyToPath(pathname, -1);
        else
            addInotifyToPath(pathname, fd);
    }
    free(queueData);
    queueData = NULL;
    queueDataIdx = 0;
    queueDataCapacity = 0;
}

int main(int argc, char **argv)
{
    if (argc == 1) {
        printf(RED "Please supply at least one directory for this program\n" NONE);
        exit(1);
    }

    inotifyData = NULL;
    inotifyDataIdx = inotifyDataCapacity = 0;

    directoryData = NULL;
    directoryDataIdx = directoryDataCapacity = 0;

    queueData = NULL;
    queueDataIdx = queueDataCapacity = 0;

    for (int i = 1; i < argc; i++) {
        search(argv[i], 1);
        attachListener();
    }

    listenForInotifyEvents();

    return 0;
}
