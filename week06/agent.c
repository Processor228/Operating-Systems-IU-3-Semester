#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define FIRST_FILE "/var/run/agent.pid"
#define SECOND_FILE "text.txt"

FILE* file_two = NULL;

void usr1_handler(int signum) {
    printf("but I am trying to !\n");
    file_two = fopen(SECOND_FILE, "r");
    if (file_two != NULL) {
        char buf;
        while ((buf = fgetc(file_two)) != EOF) {
            putchar(buf);
        }
    } else {
        printf("File os not opened\n");
    }
    fclose(file_two);
}

void usr2_handler(int signum) {
    printf("Process terminating...\n");
    fflush(stdout);
    system("rm /var/run/agent.pid");
    exit(EXIT_SUCCESS);
}


int main () {
    signal(SIGUSR1, usr1_handler);
    signal(SIGUSR2, usr2_handler);

    system("touch /var/run/agent.pid");
    /*
     * First file, writing pid
     */
    FILE* file;
    file = fopen(FIRST_FILE, "w");
    if (file == NULL) {
        printf("Error: file %s was not opened \n", FIRST_FILE);
        perror("Reason");
        return EXIT_FAILURE;
    }

    fprintf(file, "%d", getpid());
    fclose(file);

    /*
     * Second file, reading text.txt and printing to stdout
     */
    file_two = fopen(SECOND_FILE, "r");
    char buf;
    while ((buf = fgetc(file)) != EOF) {
        putchar(buf);
    }
    fclose(file_two);

    while (1)
        pause();

    return EXIT_SUCCESS;
}
