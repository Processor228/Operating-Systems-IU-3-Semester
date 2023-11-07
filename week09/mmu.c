#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>

#define FILENAME "/tmp/ex2/pagetable"
#define FILENAME_2 "pagetable"

typedef struct PTE {
// The page is in the physical memory (RAM)
    bool valid;
// The frame number of the page in the RAM
    int frame;
// The page should be written to disk
    bool dirty;
// The page is referenced/requested
    int referenced;
// ageing counter
    unsigned char counter;
// The number of references to the page
    int nfu_counter;
} PTE;

unsigned int references = 0;
unsigned int misses = 0;
short algo_type;

void operation_preamble(char op_type, int page, bool valid) {
    /*
     * Logging and benchmarking is implemented here
     */
    if (op_type == 'R') {
        printf("Read Request from page %d\n", page);
    }
    if (op_type == 'W') {
        printf("Write Request for page %d\n", page);
    }
    if (!valid) {
        printf("It is not a valid page --> page fault\n");
    } else {
        printf("It is a valid page\n");
    }
    references++;
    if (!valid) {
        misses++;
    }
}

void benchmarking_report() {
    printf("Hit ratio: %lf\n", 1 - (double)misses/(double)references);
}

void ptable_report(PTE* ptable, int pt_size) {
    printf("Page table\n");
    for (int i = 0; i < pt_size; i++) {
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d, counter=%d\n", i, ptable[i].valid, ptable[i].frame,
               ptable[i].dirty, ptable[i].referenced, ptable[i].counter);
    }
}

void handlr(int signal) {
    printf("MMU resumed by SIGCONT signal from pager\n");
    fflush(stdout);
}

short type_decision(char* arg) {
    /// Deciding on replacement strategy.
    if (strcmp(arg, "random") == 0) {
        return 1;
    }
    if (strcmp(arg, "nfu") == 0) {
        return 2;
    }
    if (strcmp(arg, "aging") == 0) {
        return 3;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("at least 3 args should be provided into the mmu!\n");
    }
    signal(SIGCONT, handlr);  // Sigcont when pager tries to resume the mmu.

    /*
     * Parsing CL args
     */
    algo_type = type_decision(argv[1]);
    int pages = atoi(argv[2]);
    int pager_pid = atoi(argv[argc-1]);
    long sz = sysconf(_SC_PAGESIZE);

    /*
     * Boring: mmap
     */
    int pt = open(FILENAME_2, O_RDWR);
    void *addr = mmap(NULL, sizeof(PTE)*pages, PROT_READ|PROT_WRITE, MAP_SHARED, pt, 0);

    if (addr == MAP_FAILED) {
        perror("mmap");
        close(pt);
        return EXIT_FAILURE;
    }
    PTE* ptable = (PTE*)addr;

    char op_type;
    int page_n;

    int idx = 2;
    while (idx < argc-1) {
        sscanf(argv[idx], "%c%d", &op_type, &page_n);
        idx++;

        operation_preamble(op_type, page_n, ptable[page_n].valid);
        /*
         * We accessed the page, now we increment it's counter. (In NFU case)
         */
        if (algo_type == 2)
            ptable[page_n].nfu_counter++;

        /*
         * We accessed the page, now we tell pager to update counters. (In Aging case)
         */
        if (algo_type == 3) {
            printf("Tell pager to update counters\n");
            ptable[page_n].referenced = getpid();
            kill(pager_pid, SIGUSR2);  /// There is a timer interrupt for each page access request.
            pause();
            ptable[page_n].referenced = 0;
        }

        if (!ptable[page_n].valid) {
            ptable[page_n].referenced = getpid();
            printf("Ask pager to load it from disk (SIGUSR1 signal) and wait\n");

            kill(pager_pid, SIGUSR1);
            pause();
        }
        if (op_type == 'W') {
            printf("It is a write request then set the dirty field\n");
            ptable[page_n].dirty = true;
        }
        if (op_type == 'R') {

        }
//        ptable_report(ptable, pages);
    }

    if (munmap(addr, sz) == -1) {
        perror("munmap");
        close(pt);
        return 1;
    }
    close(pt);
    kill(pager_pid, SIGUSR1);

    benchmarking_report();
    return EXIT_SUCCESS;
}
