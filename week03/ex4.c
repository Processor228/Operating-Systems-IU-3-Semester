#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

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
            int * variable_to_free = opr(result, (base + size * i));
            *result = *variable_to_free;
            free(variable_to_free);
        }
        return result;
    } else{
        double * result = (double *) malloc(size);
        *result = *(double *)initial_value;
        for (int i = 0; i < n; i += 1) {
            double * variable_to_free = opr(result, (base + size * i));
            *result = *variable_to_free;
            free(variable_to_free);
        }
        return result;
    };
}

int main () {
    int integers[ARRAY_SIZE] = {3, 5, 1, 9, 0};
    double doubles[ARRAY_SIZE] = {4, 5, 2, .5, .5};

    int sum = 0;
    int* summa_int = aggregate(integers, sizeof(int), ARRAY_SIZE, &sum, additionInt);

    int initial_prod_int = 1;
    int * prod_int = aggregate(integers, sizeof(int), ARRAY_SIZE, &initial_prod_int, multiplicationInt);

    double prod = 1;
    double* prod_double = aggregate(doubles, sizeof(double), ARRAY_SIZE, &prod, multiplicationDou);

    int maxim = INT_MIN;
    int* maxim_int = aggregate(integers, sizeof(int), ARRAY_SIZE, &maxim, maximumInt);

    double initial_double_sum = 0;
    double * double_sum = aggregate(doubles, sizeof(double), ARRAY_SIZE, &initial_double_sum, additionDou);

    double maxim_ = 0;
    double* maxim_dou = aggregate(doubles, sizeof(double), ARRAY_SIZE, &maxim_, maximumDou);

    printf("%d, %d, %d, %lf, %lf, %lf\n", *summa_int, *prod_int, *maxim_int, *double_sum, *prod_double, *maxim_dou);

    free(summa_int);
    free(prod_double);
    free(maxim_int);
    free(double_sum);
    free(prod_int);
    free(maxim_dou);

    return EXIT_SUCCESS;
}
