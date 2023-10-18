#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

// in bytes (chars)
#define SIZE 524288000

int main() {
    FILE* file = fopen("text.txt", "w");
    if (file == NULL) {
        perror("FILE ERROR:");
        return EXIT_FAILURE;
    }
    FILE* random = fopen("/dev/random", "r");
    if (file == NULL) {
        perror("FILE ERROR:");
        return EXIT_FAILURE;
    }

    int i = 0;
    char c;
    while (i < SIZE) {
        fscanf(random, "%c", &c);
        if (isprint(c) && c != '\n') {
            fprintf(file, "%c", c);
            i++;
            if (i % 1024 == 0) {
                fprintf(file, "%c", c);
                i++;
            }
        }
    }

    fclose(random);
    fclose(file);

    long sz = sysconf(_SC_PAGESIZE);
    int read_ = open("text.txt", O_RDWR);

    off_t end = SIZE;
    long long capitals = 0;
    for (off_t offset = 0; offset < end; offset += sz) {
        // mapping one page, and we now may treat this addr as an array of chars
        void *addr = mmap(NULL, sz, PROT_READ|PROT_WRITE, MAP_SHARED, read_, offset);
        if (addr == MAP_FAILED) {
            perror("mmap");
            close(read_);
            return EXIT_FAILURE;
        }

        char* current_line = (char*) addr;
        for (int idx = 0; idx < sz && offset + idx < SIZE; idx++) {
            if (isupper(current_line[idx])) {
                capitals++;
                current_line[idx] = tolower(current_line[idx]);
            }
        }

        if (munmap(addr, sz) == -1) {
            perror("munmap");
            close(read_);
            return 1;
        }
    }
    /*
     * Printing the evaluated number of capitals.
     */
    printf("%lld\n", capitals);

    close(read_);

    return EXIT_SUCCESS;
}
