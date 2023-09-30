#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE 120

void dot_prod(const double * u, const double * v, int n, FILE * f) {
    double result = 0;
    for (int i = 0; i < n; i += sizeof(double)) {
        result += u[i] * v[i];
    }
    // sending part of the calculations to the file
    fprintf(f, " %lf ", result);
}

int main() {
    srand(clock());
    double u [SIZE];
    double v [SIZE];
    printf("Values: u  v\n");
    for (int i = 0; i < SIZE; i++) {
        u[i] = rand() % 100;
        v[i] = rand() % 100;

        printf("%lf %lf\n", u[i], v[i]);
    }

    FILE * f = fopen("temp.txt", "w");

    int n;
    printf("Enter n, the number of processes to run this calculation in parallel(make sure n is divisor of 120): \n");
    scanf("%d", &n);
    pid_t processes[n];
    for (int i = 0; i < n; i++) {
        pid_t new_process = fork();
        if (new_process == 0) {  // if this is happening in a new-spawned process
            dot_prod(&u[i * SIZE / n], &v[i * SIZE / n], SIZE / n, f);
            exit(EXIT_SUCCESS);
        }
    }
    fclose(f);
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    FILE * file;
    file = fopen("temp.txt", "r");
    double result = 0;
    for (int i = 0; i < n; i++) {
        double temp;
        fscanf(file, "%lf", &temp);
        result += temp;
    }
    fclose(file);

    printf("%lf", result);
    return EXIT_SUCCESS;
}