#include <stdlib.h>
#include <stdio.h>
#include <string.h>

long long convert(long long n, int s, int t) {
    int decimal = 0;
    int deg = 1;

    /*
     * Converting to decimal system
     */
    while (n > 0) {
        short cur_digit = n % 10;
        n /= 10;
        if (cur_digit >= s) {
            printf("Digits in numbers should be less than base");
            return -1;
        }
        decimal += cur_digit * deg;
        deg *= s;
    }

    /*
     * Now converting into target system
     */
    long long target_num = 0;
    deg = 1;
    while (decimal > 0) {
        short cur = decimal % t;
        decimal /= t;
        target_num += deg * cur;
        deg *= 10;
    }
    return target_num;
}

int main() {
    long long n;
    int source_sys, target_sys;

    scanf("%lld %d %d", &n, &source_sys, &target_sys);

    if (n < 0) {
        printf("Number should be positive\n");
        return EXIT_FAILURE;
    }
    if (source_sys < 2 || source_sys > 10) {
        printf("Source system should belong from 2 to 10\n");
        return EXIT_FAILURE;
    }
    if (target_sys < 2 || target_sys > 10) {
        printf("Target system should belong from 2 to 10\n");
        return EXIT_FAILURE;
    }

    long long converted = convert(n, source_sys, target_sys);
    if (converted == -1) {
        return EXIT_FAILURE;
    }
    printf("%lld", converted);

    return EXIT_SUCCESS;
}
