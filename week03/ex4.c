#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define ARRAY_SIZE 5

void * additionInt(const void* a, const void* b) {
    int * res = (int*) (malloc(sizeof(int)));
    *res = *(int*)a + *(int*)b;
    return res;
}

void * additionDou(const void* a, const void* b) {
    double* res = (double*) (malloc(sizeof(double)));
    *res = *(double *)a + *(double *)b;
    return res;
}

void * multiplicationInt(const void* a, const void* b) {
    int * res = (int*) (malloc(sizeof(int)));
    *res = *(int*)a * *(int*)b;
    return res;
}

void * multiplicationDou(const void* a, const void* b) {
    double* res = (double*) (malloc(sizeof(double)));
    *res = *(double *)a * *(double *)b;
    return res;
}

void * maximumInt(const void* a, const void* b) {
    int * res = (int*) (malloc(sizeof(int)));
    int a_ = *(int*)a;
    int b_ = *(int*)b;
    *res = a_ > b_ ? a_ : b_;

    return res;
}

void * maximumDou(const void* a, const void* b) {
    double * res = (double*) (malloc(sizeof(double)));
    double a_ = *(double*)a;
    double b_ = *(double *)b;
    *res = a_ > b_ ? a_ : b_;
    return res;
}

void* aggregate(void* base, size_t size, int n, void* initial_value, void* (*opr) (const void*, const void*)) {
    if (size == sizeof(int)) {
        int* result = (int*) malloc(size);
        *result = *(int *)initial_value;
        for (int i = 0; i < n; i += 1) {
            result = opr(result, (base + size * i));
        }
        return result;
    } else{
        double * result = (double *) malloc(size);
        *result = *(double *)initial_value;
        for (int i = 0; i < n; i += 1) {
            result = opr(result, (base + size * i));
        }
        return result;
    };
}

int main () {
    int integers[5] = {3, 5, 1, 9, 0};
    double doubles[5] = {4, 5, 2, .5, .5};

    int sum = 0;
    int* summa_int = aggregate(integers, sizeof(int), ARRAY_SIZE, &sum, additionInt);

    double prod = 1;
    double* prod_double = aggregate(doubles, sizeof(double), ARRAY_SIZE, &prod, multiplicationDou);

    int maxim = INT_MIN;
    int* maxim_int = aggregate(integers, sizeof(int), ARRAY_SIZE, &maxim, maximumInt);

    double maxim_ = INT_MIN;
    double* maxim_dou = aggregate(doubles, sizeof(int), ARRAY_SIZE, &maxim_, maximumInt);

    printf("%d, %lf, %d\n", *summa_int, *prod_double, *maxim_int);

    free(summa_int);
    free(prod_double);
    free(maxim_int);

    return EXIT_SUCCESS;
}
