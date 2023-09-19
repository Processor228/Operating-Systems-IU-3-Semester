#include <stdlib.h>
#include <stdio.h>
#include <string.h>

long long tribonacci(int n) {
    long long n_1 = 0;
    long long n_2 = 1;
    long long n_3 = 1;

    if (n == 0) {
        return 0;
    }
    if (n == 1 || n == 2) {
        return 1;
    }

    for (int i = 3; i <= n; ++i) {
        long long new_n3 = n_1 + n_2 + n_3;
        long long new_n2 = n_3;
        long long new_n1 = n_2;

        n_3 = new_n3;
        n_2 = new_n2;
        n_1 = new_n1;
    }
    return n_3;
}

int main() {
//    int n;
//    fscanf(stdin, "%d", &n);
//    printf("%lld", tribonacci(n));

    printf("%lld\n", tribonacci(4));
    printf("%lld\n", tribonacci(36));

    return EXIT_SUCCESS;
}
