#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

bool is_prime (int n) {
    if (n <= 1) return false;
    int i = 2;
    for (; i * i <= n ; i ++) {
        if ( n % i == 0)
            return false;
    }
    return true;
}

// The mutex
pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
int k = 0;
int c = 0;
int n = 0;

int get_number_to_check() {
    int ret = k ;
    if (k != n)
        k ++;
    return ret ;
}

// increase prime counter
void increment_primes() {
    c++;
}

// start_routine
void * check_primes(void * arg) {
    while (true) {
        pthread_mutex_lock(&global_lock);
        int candidate = get_number_to_check();
        pthread_mutex_unlock(&global_lock);

        if (candidate >= n) {
            break;
        }

        if (is_prime(candidate)) {
            pthread_mutex_lock(&global_lock);
            increment_primes();
            pthread_mutex_unlock(&global_lock);
        }
    }
    return NULL;
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./ex4 n m\n");
        return EXIT_FAILURE;
    }
    if (argv[1] < 0 || argv[2] < 0 || argv[1] > argv[2]) {
        printf("Parameter n and m should be not less than zero, and n > m\n");
        return EXIT_FAILURE;
    }

    n = atoi(argv[1]);  // len of interval.
    int m = atoi(argv[2]);  // number of threads.

    typedef struct thread_inf {
        pthread_t tid;

    } thread_inf;

    if (pthread_mutex_init(&global_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    thread_inf trds[m];
    for (int trd = 0; trd < m; trd++)
    {
        pthread_create(&trds[trd].tid, NULL, check_primes, NULL);
    }

    for (int trd = 0; trd < m; trd++) {
        pthread_join(trds[trd].tid, NULL);
    }

    printf("number of primes on [%d, %d): %d\n", 0, n, c);
    return EXIT_SUCCESS;
}  // 2 3 5 7
