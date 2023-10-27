#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define LEN 15
#define PAS_LEN 8

int main() {
    FILE* pfile = fopen("/tmp/ex1.pid", "wr");
    if (pfile == NULL) {
        perror("File");
        return EXIT_FAILURE;
    }
    fprintf(pfile, "%d", getpid());
    fclose(pfile);

    char* password = malloc(LEN);
    strcpy(password, "pass:");

    FILE* random = fopen("/dev/random", "r");
    if (random == NULL) {
        perror("FILE ERROR:");
        return EXIT_FAILURE;
    }

    int charc = 5;
    while (charc < LEN - 2) {
        char c;
        fscanf(random, "%c", &c);
        if (isprint(c) && c != '\n' && c != '.') {
            password[charc] = c;
            charc++;
        } else {
            continue;
        }
    }
    fclose(random);

    while(1)
        pause();

    return EXIT_SUCCESS;
}
