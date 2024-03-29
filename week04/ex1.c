#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

// funny fact learnt during the lab: you can get a zombie process, if you will terminate
// the child when the parent is sleeping and can't receive the signal that a child got killed, so it becomes a zombie

/*
Write a program ex1.c which creates two new processes, any of
them should not be a child of the other (their parents should be
the “main” process).
 */
int main() {
    clock_t beginning_parent = clock();
    // cp - stands for child process
    pid_t cp_1, cp_2;

    clock_t beginning_2;
    clock_t beginning_1;
    cp_1 = fork();
    beginning_1 = clock();
    if (cp_1 == 0) { // the first child
        /*
         * Here I print ppid !
         */
        printf("from child 1: %d, ppid %d\n", getpid(), getppid());
    } else {  // the parent running
        cp_2 = fork();
        beginning_2 = clock();
        /*
         * Here I print ppid !
         */
        if (cp_2 == 0) {
            printf("from child 2: %d, ppid %d\n", getpid(), getppid());
        }
    }
    if (cp_1 != 0 && cp_2 != 0)
        printf("cp_1: %d \ncp_2: %d\n", cp_1, cp_2);

    if (cp_1 == 0) {
        clock_t time_cp_1_kia = clock();
        printf("child 1 lived for %f\n", ((float)time_cp_1_kia*1000 - (float)beginning_1*1000)/CLOCKS_PER_SEC);//*1000/CLOCKS_PER_SEC);
        exit(EXIT_SUCCESS);
    }
    if (cp_2 == 0) {
        clock_t time_cp_2_kia = clock();
        printf("child 2 lived for %f\n", ((float)time_cp_2_kia*1000 - (float)beginning_2*1000)/CLOCKS_PER_SEC);//CLOCKS_PER_SEC);
        exit(EXIT_SUCCESS);
    }

    wait(NULL);
    wait(NULL);
    clock_t time_parent_kia = clock();
    printf("Parent also exits, living only %f, and his pid actually was %d\n", ((float)time_parent_kia*1000 - (float)beginning_parent*1000)/CLOCKS_PER_SEC, getpid());

    return EXIT_SUCCESS;
}
