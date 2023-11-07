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
#define MIN_BUFFER_SIZE 2

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

typedef struct TLB_entry {
    int page;
    int frame;
} TLB;

unsigned int buffer_size;
TLB* buffer;
unsigned int references = 0;
unsigned int misses = 0;
unsigned int TLB_miss = 0;
short algo_type;

int TLB_map(const int page) {
    int frame = -1;
    int idx_frame = -1;
    /*
     * In this cycle we try to get the most recent translation of that page (it is the most right).
     */
    for (int i = 0; i < buffer_size; i++) {
        if (buffer[i].page == page) {
            idx_frame = i;
            frame = buffer[i].frame;
        }
    }
    printf("found frame %d\n", frame);
    if (frame == -1)
        return -1;
    /*
     * And then in this cycle we try to see whether this frame wasn't allocated for some other page
     */
    for (idx_frame+=1; idx_frame < misses%buffer_size; idx_frame++) {
        if (buffer[idx_frame].frame == frame) {  // if we got frame match, then it is a different page, since we already
            printf("but it's been replaced by page %d, so it is no longer in memory\n", buffer[idx_frame].page);
            return -1;  // had the rightest frame allocation of that page.
        }
    }

    return frame;
}

void update_TLB(const int page, const int frame) {
    buffer[misses%buffer_size].page = page;
    buffer[misses%buffer_size].frame = frame;
}

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
    printf("TLB miss ratio: %lf\n", (double)TLB_miss/(double)references);
}

void ptable_report(PTE* ptable, int pt_size) {
    printf("Page table\n");
    for (int i = 0; i < pt_size; i++) {
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d, counter=%d\n", i, ptable[i].valid, ptable[i].frame,
               ptable[i].dirty, ptable[i].referenced, ptable[i].counter);
    }
}

void print_tlb() {
    for (int i = 0; i < buffer_size; i++) {
        printf("page %d: frame %d\n", buffer[i].page, buffer[i].frame);
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

    /*
     * Buffer instantiation
     */
    buffer_size = pages * .2;
    if (buffer_size == 0)
        buffer_size = MIN_BUFFER_SIZE;

    printf("%d\n", buffer_size);
    buffer = malloc(sizeof(TLB) * buffer_size);
    for (int i = 0; i < buffer_size; i++) {
        buffer[i].frame = -1;
        buffer[i].page = -1;
    }

    char op_type;
    int page_n;
    int tlb_result;

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

        if ((tlb_result = TLB_map(page_n)) == -1) {
            /*
             * No such frame found in TLB.
             */
            printf("TLB miss\n");
            print_tlb();
            TLB_miss++;
        }

        if (!ptable[page_n].valid) {
            ptable[page_n].referenced = getpid();
            printf("Ask pager to load it from disk (SIGUSR1 signal) and wait\n");

            kill(pager_pid, SIGUSR1);
            pause();
            update_TLB(page_n, ptable[page_n].frame);
        }

        if (op_type == 'W') {
            printf("It is a write request then set the dirty field\n");
            ptable[page_n].dirty = true;
        }
        if (op_type == 'R') {

        }
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
