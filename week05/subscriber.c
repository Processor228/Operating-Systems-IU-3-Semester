#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define PSIZE 64

int main (int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: ./subscriber n \n");
        return EXIT_FAILURE;
    }
    if (argv[1] < 0) {
        printf("Parameter n should be not less than zero\n");
        return EXIT_FAILURE;
    }
    int n = atoi(argv[1]);
//
    int fd1;
    char path[PSIZE];
    sprintf(path, "/tmp/ex1/s%d", n);  // dont forget to add n
    char buffer[BUFFER_SIZE];
    fd1 = open(path, O_RDONLY);
    if (fd1 == -1) {
        close(fd1);
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        int c = read(fd1, buffer, sizeof(buffer));
        printf("Received: %s\n", buffer);
        if (c == 0 || c == -1) break;
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }
    close(fd1);
    return EXIT_SUCCESS;
}


