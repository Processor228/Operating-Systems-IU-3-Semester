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
        printf("Usage: ./publisher n \n");
        return EXIT_FAILURE;
    }
    if (argv[1] < 0) {
        printf("Parameter n should be not less than zero\n");
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    int pipes[n][2];

    for (int i = 0 ; i < n; i++)
        pipe(pipes[i]);
//    rmdir("/tmp/ex1");
    mkdir("/tmp/ex1", 0777);

    char path[PSIZE];
    char buffer[BUFFER_SIZE];
    for (int i = 1; i <= n; i++)
    {
        pid_t pid;
        switch (pid = fork()) {
            case -1:
                perror("Fork failure");
                exit(EXIT_FAILURE);
            case 0:
                /*child process*/
//                char * d = ";"pipa%d"
                sprintf(path, "/tmp/ex1/s%d", i); // dont forge to add i

                remove(path);
                unlink(path);

                mkfifo(path, 0777);
                puts(path);
                int fd = open(path, O_RDWR);
                puts("Opposite pipe is open");
                do {
                    close(pipes[i-1][1]);
                    read(pipes[i-1][0], buffer, BUFFER_SIZE);
                    printf("Received: %s\n", buffer);

                    if (write(fd, buffer, sizeof(buffer)) == -1)
                        break;
                } while (strcmp(buffer, "exit") != 0);
                close(fd);
                return EXIT_SUCCESS;
            default:  // parent
                break;
        }
    }
    do {
        scanf("%[^\n]%*c", buffer);

        for (int child = 0; child < n; child++) {
            close(pipes[child][0]);
            write(pipes[child][1], buffer, BUFFER_SIZE);
        }

    } while (strcmp(buffer, "exit") != 0);

    rmdir("/tmp/ex1");

    return EXIT_SUCCESS;
}


