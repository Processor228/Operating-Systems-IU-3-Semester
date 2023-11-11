#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

char path[PATH_MAX];

void construct_full_path(char* full_path, const char* source) {
    strcpy(full_path, path);
    strcpy(full_path, "/");
    strcpy(full_path, source);
}

void find_all_hlinks(const char* source) {
    char full_path[PATH_MAX];
    construct_full_path(full_path, source);

    struct stat fileStat;
    stat(full_path, &fileStat);

    unsigned long inode_interest = fileStat.st_ino;

    DIR *dir = opendir(path);

    if (dir == NULL) {
        printf("Could not open directory %s\n", path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        struct stat file;
        char fullpath[PATH_MAX];
        strcpy(fullpath, path);
        strcat(fullpath, "/");
        strcat(fullpath, entry->d_name);

        if (!stat(fullpath, &file)) {
            if (fileStat.st_ino == inode_interest) {
                printf("File: %s\n", entry->d_name);
                printf("Path: %s\n", fullpath);
                printf("Contents: \n");
                FILE* fp = fopen(fullpath ,"r");
                char buffer[256];
                fscanf(fp, "%s", buffer);
                printf("%s\n", buffer);
                fclose(fp);
            }
        } else {
            perror("Error in stat");
        }
    }
}

void unlink_all(const char* source) {
    char full_path[PATH_MAX];
    construct_full_path(full_path, source);

    struct stat fileStat;
    stat(full_path, &fileStat);

    unsigned long inode_interest = fileStat.st_ino;

    DIR *dir = opendir(path);

    if (dir == NULL) {
        printf("Could not open directory %s\n", path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        struct stat file;
        char fullpath[PATH_MAX];
        strcpy(fullpath, path);
        strcat(fullpath, "/");
        strcat(fullpath, entry->d_name);

        if (!stat(fullpath, &file)) {
            if (file.st_ino == inode_interest && strcmp(entry->d_name, source) != 0) {
                printf("File: %s is to be deleted\n", entry->d_name);
                remove(fullpath);
            }
        } else {
            perror("Error in stat");
        }
    }
    printf("The path to the last hard link is %s\n", full_path);
}

void create_sym_link(const char* source, const char* link) {
    char full_path[PATH_MAX];
    construct_full_path(full_path, source);

    symlink(source, link);
}

void create_hard_link(const char* source, const char* _link) {
    char full_path[PATH_MAX];
    construct_full_path(full_path, source);

    link(source, _link);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: %s /path/to/smt\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    strcpy(path, argv[1]);
//    strcpy(path, "./");

    /*
     * Create a file myfile1.txt that contains text "Hello world."
     */
    FILE* fp = fopen("myfile1.txt" ,"w");
    fprintf(fp, "Hello world.");
    fclose(fp);

    /*
     *  Create two hard links myfile11.txt and myfile12.txt, add them to the watched directory
     */
    create_hard_link("myfile1.txt", "myfile11.txt");
    create_hard_link("myfile1.txt", "myfile12.txt");

    /*
     *  Find all hard links to myfile1.txt and print their i-nodes, paths, and content.
     */
    find_all_hlinks("myfile1.txt");

    /*
     *  Move the file myfile1.txt to another folder /tmp/myfile1.txt.
     */
    char myfile1_path[PATH_MAX];
    construct_full_path(myfile1_path, "myfile1.txt");
    rename(myfile1_path, "/tmp/myfile1.txt");

    /*
     *  Modify the file myfile11.txt (its content). Did the monitor.c
     *  program reported an event for myfile11.txt? Justify your answer
     *  and add it to ex1.txt.
     */
    fp = fopen("myfile11.txt" ,"w");
    fprintf(fp, "Hello world, Brother !\n");
    fclose(fp);

    /*
     *  Create a symbolic link myfile13.txt in the watched directory to
     *  /tmp/myfile1.txt. Modify the file /tmp/myfile1.txt (its content)
     *  Did the monitor.c program reported an event for myfile13.txt?
     *  Justify your answer and add it to ex1.txt.
     */
    create_sym_link("/tmp/myfile1.txt", "myfile13.txt");
    fp = fopen("/tmp/myfile1.txt" ,"w");
//    fp = fopen("myfile13.txt", "w");
    fprintf(fp, "Hello world, Brother ! x2\n");
    fclose(fp);

    /*
     *  Remove all duplicates of hard links to myfile11.txt only in the
     *  watched directory. After this, print the stat info of the kept hard
     *  link. Check the number of hard links and explain the difference.
     */
    unlink_all("myfile11.txt");

    struct stat inf;
    stat("myfile11.txt", &inf);
    printf("Size: %ld\n", inf.st_size);
    printf("Permissions: %o\n", inf.st_mode & 0777);
    printf("Number of links: %ld\n", inf.st_nlink);
    printf("Owner: %d\n", inf.st_uid);
    printf("Group: %d\n", inf.st_gid);
    printf("Device: %ld\n", inf.st_dev);
    printf("Inode: %ld\n", inf.st_ino);
    printf("Access time: %s", ctime(&inf.st_atime));
    printf("Modification time: %s", ctime(&inf.st_mtime));
    printf("Change time: %s", ctime(&inf.st_ctime));

    return EXIT_SUCCESS;
}
