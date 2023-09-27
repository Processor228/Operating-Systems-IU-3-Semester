#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME_MAX_LEN 64
#define PATH_MAX_LEN 2048
#define FILE_DATA_MAX_LEN 1024
#define DIRECTORIES_MAX 256

struct File;
struct Directory;

struct Directory {
    char name[FILENAME_MAX_LEN];
    struct Directory* directories[DIRECTORIES_MAX];
    struct File* files[DIRECTORIES_MAX];
    char Path[PATH_MAX_LEN];
    unsigned char nf;
    unsigned char nd;
};

struct File {
    int id;
    char name[FILENAME_MAX_LEN];
    int size;
    char data[FILE_DATA_MAX_LEN];
    struct Directory* directory;
};

void overwrite_to_file(struct File* self, const char* data) {
    strcpy(self->data, data);
    self->size = strlen(data);
}

void append_to_file(struct File* self, const char* data) {
    strcat(self->data, data);
    self->size = strlen(self->data);
}

void printp_file(struct File* file) {
    printf("%s/%s\n", file->directory->Path, file->name);
}

void add_file(struct File* file, struct Directory* dir) {
    dir->files[dir->nf++] = file;
    file->directory = dir;
}

// Adds the subdirectory dir1 to the directory dir2
void add_dir(struct Directory* dir1, struct Directory* dir2) {
    /*
     * THIS CODE IS COPIED FROM THE LAB PRESENTATION
     */

    if (dir1 && dir2){
        dir2->directories[dir2->nd] = dir1;
        dir2->nd++;
        char temp_path[PATH_MAX_LEN];
        if (strcmp(dir2->Path, "/")) {
            strcpy(temp_path, dir1->Path);
            strcat(temp_path, "/");
            strcat(temp_path, dir1->name);
            strcpy(dir1->Path, temp_path);
        } else {
            strcpy(temp_path, "/");
            strcat(temp_path, dir1->name);
            strcpy(dir1->Path, temp_path);
        }
    }
}

int main () {
    struct Directory root = {"/"};
    struct Directory home = {"home"};
    struct Directory bin = {"bin"};

//    add_subdir(&root, &home);
//    add_subdir(&root, &bin);

    add_dir(&home, &root);
    add_dir(&bin, &root);

    struct File bash = {
            1,
            "bash"
    };

    add_file(&bash, &bin);

    struct File ex3_1_c = {
            0,
            "ex3_1.c",
            0,
            };
    struct File ex3_2_c = {
            0,
            "ex3_2.c",
            0,
    };

    overwrite_to_file(&ex3_1_c, "int printf(const char * format, ...);");
    overwrite_to_file(&ex3_2_c, "//This is a comment in C language");

    add_file(&ex3_1_c, &home);
    add_file(&ex3_2_c, &home);
    append_to_file(bin.files[0], "Bourne Again Shell!!");
    append_to_file(home.files[0], "int main(){printf(”Hello World!”)}");

    printp_file(&bash);
    printp_file(&ex3_1_c);
    printp_file(&ex3_2_c);

    // here i do not free memory, since everything is allocated on the stack.

    return EXIT_SUCCESS;
}