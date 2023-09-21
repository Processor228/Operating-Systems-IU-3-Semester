#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct Point {
    double x;
    double y;
};

double distance(struct Point a, struct Point b) {
    return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
}


double abs_(double x) {
    return x > 0 ? x : -x;
}


double area_triangle(struct Point a, struct Point b, struct Point c) {
    return .5 * abs_(a.x * b.y - b.x * a.y + b.x * c.y - c.x * b.y + c.x * a.y - a.x * c.y);
}


int main () {
    struct Point A = {2.6, 6};
    struct Point B = {1, 2.2};
    struct Point C = {10, 6};

    printf("|AB| : %lf\n", distance(A, B));
    printf("S(ABC): %lf\n", area_triangle(A, B, C));

    return EXIT_SUCCESS;
}
