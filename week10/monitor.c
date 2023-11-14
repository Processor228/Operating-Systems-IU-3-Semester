#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int fd;
int wd;
char path[PATH_MAX];

void print_all_info() {
    DIR *dir = opendir(path);

    if (dir == NULL) {
        printf("Could not open directory %s\n", path);
        return;
    }

    printf("Info about all entries in the directory\n");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        struct stat fileStat;
        char fullpath[PATH_MAX];
        strcpy(fullpath, path);
        strcat(fullpath, "/");
        strcat(fullpath, entry->d_name);


        printf("-------------------------------\n");
        if (!stat(fullpath, &fileStat)) {
            printf("File: %s\n", entry->d_name);
            printf("Size: %ld\n", fileStat.st_size);
            printf("Permissions: %o\n", fileStat.st_mode & 0777);
            printf("Number of links: %ld\n", fileStat.st_nlink);
            printf("Owner: %d\n", fileStat.st_uid);
            printf("Group: %d\n", fileStat.st_gid);
            printf("Device: %ld\n", fileStat.st_dev);
            printf("Inode: %ld\n", fileStat.st_ino);
            printf("Access time: %s", ctime(&fileStat.st_atime));
            printf("Modification time: %s", ctime(&fileStat.st_mtime));
            printf("Change time: %s", ctime(&fileStat.st_ctime));
            printf("-------------------------------\n");
        } else {
            perror("Error in stat");
        }
    }
}

void terminate(int signal) {
    inotify_rm_watch(fd, wd);

    /*closing the INOTIFY instance*/
    close(fd);

    /*
     * Printing the stat info of all entries in the directory
     */
    print_all_info();

    exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: %s /path/to/smt\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    signal(SIGTERM, terminate);
    signal(SIGINT, terminate);

    strcpy(path, argv[1]);

    char buffer[EVENT_BUF_LEN];
    fd = inotify_init();

    if (fd < 0) {
        perror("inotify_init");
    }
    /*
     * program should be able to print the stat info of all
     * files and folders in the path (path) once on the startup, and once
     * before termination.
     */
    print_all_info();

    wd = inotify_add_watch(fd, argv[1], IN_CREATE|IN_DELETE|IN_MODIFY|IN_ACCESS|IN_OPEN|IN_ATTRIB);
    /*read to determine the event change happens on “/tmp” directory. Actually this read blocks until the change event occurs*/

    do {
        int length = read(fd, buffer, EVENT_BUF_LEN);

        if (length < 0) {
            perror("read");
        }
        int i = 0;

        /*actually read return the list of change events happens. Here, read the change event one by one and process it accordingly.*/
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->len) {
                if (event->mask & IN_CREATE) {
                    if (event->mask & IN_ISDIR) {
                        printf("New directory %s created.\n", event->name);
                    } else {
                        printf("New file %s created.\n", event->name);
                    }
                }
                if (event->mask & IN_DELETE) {
                    if (event->mask & IN_ISDIR) {
                        printf("Directory %s deleted.\n", event->name);
                    } else {
                        printf("File %s deleted.\n", event->name);
                    }
                }
                if (event->mask & IN_MODIFY) {
                    if (event->mask & IN_ISDIR) {
                        printf("Directory %s modified.\n", event->name);
                    } else {
                        printf("File %s modified.\n", event->name);
                    }
                }
                if (event->mask & IN_ACCESS) {
                    if (event->mask & IN_ISDIR) {
                        printf("Directory %s accessed.\n", event->name);
                    } else {
                        printf("File %s accessed.\n", event->name);
                    }
                }
                if (event->mask & IN_OPEN) {
                    if (event->mask & IN_ISDIR) {
                        printf("Directory %s .\n", event->name);
                    } else {
                        printf("File %s accessed.\n", event->name);
                    }
                }
                if (event->mask & IN_ATTRIB) {
                    if (event->mask & IN_ISDIR) {
                        printf("Directory %s metadata was changed.\n", event->name);
                    } else {
                        printf("File %s metadata was changed.\n", event->name);
                    }
                }
            }
            i += EVENT_SIZE + event->len;

            /*
             * Here we log info about the file that was engaged in event
             */
            printf("Stat info\n");
            struct stat fileStat;
            char fullpath[PATH_MAX];
            strcpy(fullpath, path);
            strcat(fullpath, "/");
            strcat(fullpath, event->name);

            printf("-------------------------------\n");
            if (!stat(fullpath, &fileStat)) {
                printf("File: %s\n", event->name);
                printf("Size: %ld\n", fileStat.st_size);
                printf("Permissions: %o\n", fileStat.st_mode & 0777);
                printf("Number of links: %ld\n", fileStat.st_nlink);
                printf("Owner: %d\n", fileStat.st_uid);
                printf("Group: %d\n", fileStat.st_gid);
                printf("Device: %ld\n", fileStat.st_dev);
                printf("Inode: %ld\n", fileStat.st_ino);
                printf("Access time: %s", ctime(&fileStat.st_atime));
                printf("Modification time: %s", ctime(&fileStat.st_mtime));
                printf("Change time: %s", ctime(&fileStat.st_ctime));
                printf("-------------------------------\n");
            } else {
                perror("Error in stat");
            }

        }
    } while (true);

    return EXIT_SUCCESS;
}
