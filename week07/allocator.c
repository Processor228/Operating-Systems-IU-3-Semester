#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 256
#define HEAP_SIZE 10000000
int memory[(int) HEAP_SIZE];

void allocate_FF(unsigned adrs, unsigned size) {
    int l = 0;
    int r = 0;
    while (r - l < size) {  // while didn't find a large enough area
        if (r > HEAP_SIZE) { // if out of memory - quit
            printf("Can't allocate\n");
            return;
        }
        r++;
        if (memory[r] == 0) {

        }
        else {
            l = r;
        }
    }

    for (int i = l; i < r; i++) {
        memory[i] = adrs;
    }

}

void allocate_BF(unsigned adrs, unsigned size) {
    /*
     * Searching for smallest suitable spot possible
     */
    int best_fit = HEAP_SIZE;
    int address_bf = -1;
    int l = 0, r = 0;
    while (r < HEAP_SIZE) {
        r++;
        if (memory[r] == 0 && r != HEAP_SIZE - 1) {

        } else {  // so if memory on this idx is non-empty, we check whether the space from l to r is good
            if (r - l >= size) {
                if (abs((r - l) - (int)size) < best_fit) {
                    best_fit = abs(r - l - (int) size);
                    address_bf = l;
                }
            }
            l = r;
        }
    }

    if (address_bf == -1) {
        printf("Can't allocate!\n");
        return;
    }

    for (int cell = address_bf; cell < size; cell++) {
        memory[cell] = adrs;
    }
}

void allocate_WF(unsigned adrs, unsigned size) {
    /*
     * Searching for largest suitable spot possible.
     */
    int best_fit = 0;
    int address_bf = -1;
    int l = 0, r = 0;
    while (r < HEAP_SIZE) {
        r++;
        if (memory[r] == 0 && r != HEAP_SIZE - 1) {

        } else {  // // so if memory on this idx is non-empty, we check whether the space from l to r is good
            if (r - l >= size) {
                if (abs((r - l) - (int)size) > best_fit) {
                    best_fit = abs(r - l - (int) size);
                    address_bf = l;
                }
            }
            l = r;
        }
    }

    if (address_bf == -1) {
        printf("Can't allocate!\n");
        return;
    }

    for (int cell = address_bf; cell < size; cell++) {
        memory[cell] = adrs;
    }
}

void clear(unsigned adrs) {
    for (int cell = 0; cell < HEAP_SIZE; cell++) {
        if (memory[cell] == adrs) {
            memory[cell] = 0;
        }
    }
}

int main () {
    memset(memory, 0, HEAP_SIZE * sizeof(int));

    FILE* queries = fopen("queries.txt", "r");
    if (queries == NULL) {
        perror("file");
        return EXIT_FAILURE;
    }

    char buffer[BUFFER_SIZE];
    time_t start = clock();
    int operation_count = 0;
    do {
        fscanf(queries, "%s", buffer);
        operation_count++;
        if (strcmp(buffer, "allocate") == 0) {
            int addr, size;
            fscanf(queries, "%d %d", &addr, &size);
            allocate_FF(addr, size);

//            allocate_BF(addr, size);
//            allocate_WF(addr, size);
        }
        if (strcmp(buffer, "clear") == 0) {
            int addr;
            fscanf(queries, "%d", &addr);
            clear(addr);
        }
    } while(strcmp(buffer, "end") != 0);
    time_t end = clock();
    fclose(queries);

    double exec_time = ((double)end - (double)start)/CLOCKS_PER_SEC;
    printf("%lf\n", exec_time);
    printf("Throughput: %f op/sec", (operation_count) / exec_time);

    return EXIT_SUCCESS;
}

