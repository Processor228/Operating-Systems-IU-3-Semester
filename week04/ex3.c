#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int n = atoi(argv[1]);

    for(int i = 0; i < n; i++) {
        pid_t child = fork();
        sleep(5);
    }

    return EXIT_SUCCESS;
}