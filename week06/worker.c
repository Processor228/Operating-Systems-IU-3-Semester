#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

// 6 digits for big triangular numbers like 113050
#define TRI_BASE 1000000

// current process pid (which executed this program)
pid_t pid;

// current process idx (starts from 0)
int process_idx;

// number of triangular numbers found so far
long tris;

bool is_triangular(long n){
    for (long i = 1; i <= n; i++){
        if (i * (i + 1) == 2 * n){
            return true;
        }
    }
    return false;
}

void signal_handler(int signum){

    // print info about number of triangulars found.
    printf("Process %d (PID=<%d>): count of triangulars found so far is %ld\n", process_idx, pid, tris);

    switch(signum) {
        case SIGTSTP:
            // pause the process indefinitely
            printf("Process %d: stopping....\n", process_idx);
            pause();
            break;
        case SIGCONT:
            // continue the process
            printf("Process %d: resuming....\n", process_idx);
            break;
        case SIGTERM:
            // terminate the process
            printf("Process %d: terminating....\n", process_idx);
            exit(EXIT_SUCCESS);
            break;
        default:
            break;
    }
}

// generates a big number n
long big_n() {
    time_t t;
    long n = 0;
    srand((unsigned) time(&t));
    while(n < TRI_BASE)
        n += rand();
    return n % TRI_BASE;
}

int main(int argc, char *argv[]){

    process_idx = atoi(argv[1]);

    pid = getpid();

    // TODO: register the signals
    signal(SIGTSTP, signal_handler);
    signal(SIGCONT, signal_handler);
    signal(SIGTERM, signal_handler);

    // The first message after creating the process
    printf("Process %d (PID=<%d>): has been started\n", process_idx, pid);
    printf("Process %d (PID=<%d>): will find the next trianguar number from (%ld, inf)\n", process_idx, pid, tris);
    long next_n = big_n() +1;

    // initialize counter
    tris = 0;

    while (true){
        if(is_triangular(next_n)) {
            printf("Process %d (PID=<%d>): I found this triangular number %ld \n", process_idx,
                   pid, next_n);
            tris++;
        }
        next_n++;
    }

}