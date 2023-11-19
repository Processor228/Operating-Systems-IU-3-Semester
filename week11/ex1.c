#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

int main() {
    DIR *d;
    struct dirent *dir;
    d = opendir("/");

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }

    return EXIT_SUCCESS;
}