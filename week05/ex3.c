#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

// primality test
bool is_prime (int n) {
    if (n <= 1) return false;
    int i = 2;
    for (; i * i <= n ; i ++) {
        if ( n % i == 0)
            return false;
    }
    return true;
}

// Primes counter in [a,b)
int primes_count(int a, int b) {
    int ret = 0;
    for (int i = a; i < b; i++)
        if (is_prime(i) != 0)
            ret++;
    return ret;
}

// argument to test routine of the thread
typedef struct prime_request {
    int a, b;
    pthread_t tid;
    long n_primes;
} prime_request;


void * prime_counter (void* arg)
{
    prime_request* req = (prime_request*) arg;
    long count = primes_count(req->a, req->b);
    req->n_primes = count;

    return (void*) count;
}

int main (int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./ex3 n m\n");
        return EXIT_FAILURE;
    }
    if (argv[1] < 0 || argv[2] < 0 || argv[1] > argv[2]) {
        printf("Parameter n and m should be not less than zero, and n > m\n");
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);  // len of interval.
    int m = atoi(argv[2]);  // number of threads.

    prime_request pqs[m];

    int len_of_interval = ceil(n / m);
    for (int trd = 0; trd < m; trd++)
    {
        pqs[trd].a = trd * len_of_interval;

        if (trd == m - 1) {  // if the last interval
            pqs[trd].b = n;
        } else {
            pqs[trd].b = trd * len_of_interval + len_of_interval;
        }

        pthread_create(&pqs[trd].tid, NULL, prime_counter, &pqs[trd]);
    }

    for (int trd = 0; trd < m; trd++) {
        pthread_join(pqs[trd].tid, NULL);
    }

    long res = 0;
    for (int i = 0; i < m; i++)
        res += pqs[i].n_primes;

    printf("number of primes on [%d, %d): %ld\n", 0, n, res);

    /*
     * Since nothing is allocated on the heap,
     * I have nothing to free().
     */

    return EXIT_SUCCESS;
}  // 2 3 5 7 11 13 17 19
