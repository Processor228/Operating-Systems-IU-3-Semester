#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main () {

    char buffer[BUFFER_SIZE];
    pid_t pid;
    int fd[2];
    pipe(fd);

    switch(pid = fork()){
        case -1:
            perror("Fork failure");
            exit(1);
        case 0:
            /*child process*/
            close(fd[1]);
            read(fd[0], buffer, BUFFER_SIZE);
            printf("Received: %s\n", buffer);
            exit(1);
            break;
        default:
            /*parent process*/
            scanf("%[^\n]%*c", buffer);

            close(fd[0]);
            write(fd[1], buffer, BUFFER_SIZE);
            break;
    }

    return EXIT_SUCCESS;
}


