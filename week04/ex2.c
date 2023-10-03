#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

#define SIZE 120

void dot_prod(const int * u, const int * v, int beg, int n) {
    int result = 0;
    FILE * f = fopen("temp.txt", "a");
    for (int i = beg; i < beg + n; ++i) {
        result += u[i] * v[i];
    }
    // sending part of the calculations to the file
    fprintf(f, "%d\n", result);
    fclose(f);
}

int main() {
    fclose(fopen("temp.txt", "w"));
    srand(clock());
    int u [SIZE];
    int v [SIZE];
    printf("Values: u  v\n");
    for (int i = 0; i < SIZE; i++) {
        u[i] = rand() % 100;
        v[i] = rand() % 100;
//        printf("%d %d\n", u[i], v[i]);
    }

    int n;
    printf("Enter n, the number of processes to run this calculation in parallel(make sure n is divisor of 120): \n");
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        pid_t new_process = fork();
        if (new_process == 0) {  // if this is happening in a new-spawned process
            dot_prod(u, v, SIZE/n * i, SIZE / n);
            exit(EXIT_SUCCESS);
        }
    }
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    FILE * file;
    file = fopen("temp.txt", "r");
    int result = 0;
    for (int i = 0; i < n; i++) {
        int temp;
        fscanf(file, "%d", &temp);
        result += temp;
    }
    fclose(file);

    int res = 0;
    for (int i = 0; i < SIZE; i++) {
        res += v[i] * u[i];
    }
    printf("%d", result);
    assert(res == result);

    return EXIT_SUCCESS;
}