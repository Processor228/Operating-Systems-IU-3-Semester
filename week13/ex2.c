#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int p = 5;  // processes
int r = 3;  // resources


bool can_be_satisfied(const int* proc, const int* avail) {
    for (int i = 0; i < r; i++) {
        if (proc[i] > avail[i])
            return false;
    }
    return true;
}

bool check_some_unmarked(const bool* marked) {
    for (int i = 0; i < p; i++) {
        if (marked[i] == false)
            return true;
    }
    return false;
}

void read_digits(FILE* f, int* buf) {
    int d;
    fscanf(f, "%d", &d);
    for (int i = r-1; i >= 0; i--) {
        buf[i] = d % 10;
        d /= 10;
    }
}


int main() {
    int resources[r];
    int available[r];
    FILE* f = fopen("input.txt", "r");

    char c;
    read_digits(f, resources);

    read_digits(f, available);

    int alloc[p][r];
    int req[p][r];

    for (int proc = 0; proc < p; proc++) {
        read_digits(f, alloc[proc]);
    }

    for (int proc = 0; proc < p; proc++) {
        read_digits(f, req[proc]);
    }

    bool marked[p];
    for (int i = 0; i < p; i++)
        marked[i] = false;

    while (true) {
        bool found_to_satisfy = false;
        for (int i = 0; i < p; i++) {
            if (marked[i])
                continue;

            if (can_be_satisfied(req[i], available)) {
                found_to_satisfy = true;
                for (int j = 0; j < r; j++) {
                    /*
                     * releasing the resources.
                     */
                    available[j] += alloc[i][j];
                }
                marked[i] = true;
            }
        }

        if (!found_to_satisfy) {
            break;
        }
    }

    if (check_some_unmarked(marked)) {
        printf("Deadlock detected !\n");
        return EXIT_SUCCESS;
    }
    printf("No deadlocks !\n");

    return EXIT_SUCCESS;
}
