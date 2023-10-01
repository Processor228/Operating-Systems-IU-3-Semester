#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define SIZE 256
#define ARGSIZE 10

void prefix(char* cwd, char usr[], char input[]);

int main() {
    char * arguments[ARGSIZE];
    char* usr = getlogin();
    char cwd [SIZE];
    char input [SIZE];

    do {
        prefix(cwd, usr, input);

        char* cmd = malloc(SIZE);
        char *pch = strtok(input, " ,.");
        strcpy(cmd, pch);

        arguments[0] = cmd;
        u_char is_background = 0;
        int n = 1;
        while (pch != NULL)
        {
            pch = strtok(NULL, " ,.");
            if (pch == NULL) {
                arguments[n] = NULL;
                n++;
                break;
            } else {
                if (strcmp(pch, "&") == 0) {
                    is_background = 1;
                    continue;
                }
            }
            char * arg = malloc(sizeof(strlen(pch)));
            strcpy(arg, pch);
            arguments[n] = arg;
            n++;
        }

        pid_t slave = fork();
        if (slave == 0) {
            char path[SIZE] = "/bin/";
            strcat(path, cmd);
            char *newenviron[] = { NULL };

            execve(path, arguments, newenviron);
        } else {
            int stat;
            if (!is_background)
                waitpid(slave, &stat, 0);

            for (int i = 0; i < n; i++) { // deallocating the strings
                free(arguments[i]);
                arguments[i] = NULL;
            }
        }
    } while(strcmp(input, "exit"));

    return EXIT_SUCCESS;
}


void prefix(char cwd[], char usr[], char input[]) {
    getcwd(cwd, SIZE);
    printf("%s:%s$", usr, cwd);
    scanf("%[^\n]%*c", input);
}
