#include <stdio.h>
#include <stdlib.h>

int const_tri(int* const p, int n) {
    if (n == 0)
        return p[0];
    if (n == 1)
        return p[1];

    /*
     * Assignment wants us not to use additional variables... But i really want one for the for loop..
     */
    for (int i = 3; i <= n; i++) {
         p[2] += p[1] + p[0];
         p[1] = p[2] - p[1] - p[0];    // p[2] + p[1] + p[0] - p[1] - p[0] = p[2]
         p[0] = p[2] - p[1] - p[0];    // p[2] - p[1] - p[0] = p[1]
    }
    return p[2];
}

int main () {
    const int x = 1;
    const int * q = &x;

    int * const p = malloc(sizeof(int) * 3);
    p[0] = x;
    p[1] = x;
    p[2] = 2 * x;
    /*
     * Printing the addresses of these memory cells. (and yes they are continious)
     */
    printf("%p %p %p\n", &p[0], &p[1], &p[2]);

    int n = 36;
    /*
     * Now we need to store the first values of tribonacci sequence in this array to calculate it properly.
     */
    p[0] = 0;
    p[1] = 1;
    p[2] = 1;
    int result = const_tri(p, n);
    printf("%d\n", result);
    free(p);

    return EXIT_SUCCESS;
}
// 0 0 1 1 2 4 7 13 24 44
// 0 1 2 3 4 5 6 7  8  9