#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/time.h>

#define PAGE_SIZE 16
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

PTE* ptable;
int pt;
char** RAM;
char** DISK;
int pages, frames;
int DISC_ACCESSES = 0;
short algo_type;

/*
 * Frame -> Page,
 * Page -> Frame is done easily with ptable[idx].frame
 */
int map_frame_to_page(int frame) {
    int victim_page = -1;
    for (int i = 0; i < pages; i++) {  // try to find out what page it is
        if (ptable[i].frame == frame) {
            victim_page = i;
        }
    }
    return victim_page;
}

void print_ptable() {
    printf("Page table\n");
    for (int i = 0; i < pages; i++) {
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n", i, ptable[i].valid, ptable[i].frame,
               ptable[i].dirty, ptable[i].referenced);
    }
}

void print_DISK() {
    printf("Disk array\n");
    for (int i = 0; i < pages; i++) {
        printf("Page %d ---> %s, is at frame %d\n", i, DISK[i], ptable[i].frame);
    }
}

void print_RAM() {
    printf("RAM array\n");
    for (int i = 0; i < frames; i++) {
        printf("Frame %d ---> %s\n", i, RAM[i]);
    }
}

void terminate() {
    printf("%d disk accesses in total\n", DISC_ACCESSES);
    close(pt);  // closing the pipe
    /*
     * Freeing memory
     */
    free(DISK);
    free(RAM);
    exit(EXIT_SUCCESS);
}

void ageing_interrupt(int signal) {
    pid_t pid_mmu = 0;
    for (int idx = 0; idx < pages; idx++) {
        if (ptable[idx].referenced != 0) {
            pid_mmu = ptable[idx].referenced;
        }
        bool r_bit = ptable[idx].referenced;
        ptable[idx].counter = (ptable[idx].counter >> 1) | (r_bit << 7);
//        printf("%d : %d, cause %d\n", idx, ptable[idx].counter, ptable[idx].referenced);
    }
    kill(pid_mmu, SIGCONT);
}

// Random page replacement, returns the PAGE number to evict (not FRAME)
int my_random(struct PTE* page_table) {
    int victim = rand() % frames;  // chosen a random frame.
    printf("Choose a random victim frame %d\n", victim);
    int victim_page = map_frame_to_page(victim);

    return victim_page;
}

// NFU page replacement, returns the PAGE number to evict (not FRAME)
int nfu(struct PTE* page_table) {
    /*
     * For replacement, choose the page with the lowest counter
     */
    int victim;
    int c_min = INT_MAX;
    for (int idx = 0; idx < pages; idx++) {
        if (ptable[idx].valid && ptable[idx].nfu_counter < c_min) {
            c_min = ptable[idx].nfu_counter;
            victim = idx;
        }
    }
    return victim;
}

// Aging page replacement, returns the PAGE number to evict (not FRAME)
int aging(struct PTE* page_table) {
    /*
     * Pretty much same strategies, the only difference is how aging updates counter.
     */
    int victim;
    int c_min = INT_MAX;
    for (int idx = 0; idx < pages; idx++) {
        if (ptable[idx].valid && ptable[idx].counter < c_min) {
            c_min = ptable[idx].counter;
            victim = idx;
        }
    }
    return victim;
}

void swap(int signal) {
    int page_requested_idx = -1;
    int mmu_pid = 0;
    for (int i = 0; i < pages; i++) {
        if (ptable[i].referenced != 0) {
            page_requested_idx = i;  // found idx of page to be swapped into RAM !
            printf("Page %d is referenced\n", i);
        }
    }

    /*
     * If no request was found, stop, this signal is for termination
     */
    if (page_requested_idx == -1) {
        terminate();
    }

    mmu_pid = ptable[page_requested_idx].referenced;  // referenced contains pid of mmu
    int victim_frame = -1;
    int victim_page = -1;
    printf("A disk access request from MMU Process (pid=%d)\n", mmu_pid);
    /*
     * Now we will try to find a free spot in the RAM. In this case, we wouldn't need any replacement strategy.
     */
    for (int RAM_frame = 0; RAM_frame < frames; RAM_frame++) {
        if (strcmp(RAM[RAM_frame], "") == 0) {  // found empty spot in RAM ?
            printf("We can allocate it to free frame %d\n", RAM_frame);
            victim_frame = RAM_frame;
            break;
        }
    }
    if (victim_frame == -1) {
        /*
         * At this moment we know that there are no free pages in RAM, so we have to
         * Evict some page. Using one of the strategies, we will select it.
         */
        switch (algo_type) {
            case 1:
                victim_page = my_random(ptable);
                break;
            case 2:
                victim_page = nfu(ptable);
                break;
            case 3:
                victim_page = aging(ptable);
                break;
            default:
                printf("Algorithm unimplemented\n");
                terminate();
        }
        victim_frame = ptable[victim_page].frame;
        printf("Replace/Evict it with page %d to be allocated to frame %d\n", page_requested_idx, victim_frame);
        /*
         * In case if the page replaced is dirty, we need to copy the data from it to DISK.
         */
        if (ptable[victim_page].dirty) {
            DISC_ACCESSES++;
            ptable[victim_frame].dirty = 0;
            printf("Copy the data from RAM frame %d into DISK page %d \n", victim_frame, victim_page);
        }
    }

    /*
     * Updating the page table...
     */
    if (victim_page != -1) { // if we have evicted something, we need to update it's PTE too.
        ptable[victim_page].frame = -1;
        ptable[victim_page].referenced = 0;
        ptable[victim_page].valid = false;
        ptable[victim_page].dirty = false;
        ptable[victim_page].counter = 0;
        ptable[victim_page].nfu_counter = 0;
    }

    ptable[page_requested_idx].frame = victim_frame;
    ptable[page_requested_idx].valid = true;
    ptable[page_requested_idx].referenced = 0;
    ptable[page_requested_idx].dirty = false;

    /*
     * Loading into memory...
     */
    printf("Copy data from the disk (page=%d) to RAM(frame=%d)\n", page_requested_idx, victim_frame);
    strcpy(RAM[victim_frame], DISK[page_requested_idx]);

    DISC_ACCESSES++;  // logging accesses...
    printf("disk accesses is %d so far\n", DISC_ACCESSES);
    print_RAM();

    /*
     * Resuming the MMU...
     */
    printf("Resume MMU process\n");
    fflush(stdout);
    kill(mmu_pid, SIGCONT);
}

int main(int argc, char* argv[]) {
    /*
     * Checking whether the inputs are correct.
     */
    if (argc != 4) {
        printf("Supply 3 args!\n");
        return EXIT_FAILURE;
    }

    srand(0);  // setting randomness
    signal(SIGUSR1, swap);  // register signal handling
    /*
     * Parsing CL args.
     */

    pages = atoi(argv[1]);
    frames = atoi(argv[2]);

    /// Deciding on replacement strategy.
    if (strcmp(argv[3], "random") == 0) {
        algo_type = 1;
        printf("Random selected\n");
    }
    if (strcmp(argv[3], "nfu") == 0) {
        algo_type = 2;
        printf("NFU selected\n");
    }
    if (strcmp(argv[3], "aging") == 0) {
        algo_type = 3;
        printf("Aging selected\n");
        signal(SIGUSR2, ageing_interrupt);
    }

    mkdir("/tmp/ex2/", 777);

    /*
     * Boring stuff: opening file, mmaping it.
     */
    pt = open(FILENAME_2, O_RDWR|O_CREAT, S_IRUSR | S_IWUSR);
    ftruncate(pt, sizeof(PTE) * pages);
    void *addr = mmap(NULL, sizeof(PTE)*pages, PROT_READ|PROT_WRITE, MAP_SHARED, pt, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    ptable = (PTE*)addr;  // now we are going to treat this memory as page table.
    for (int i = 0; i < pages; i++) {  // initializing
        ptable[i].dirty = 0;
        ptable[i].frame = -1;
        ptable[i].valid = 0;
        ptable[i].referenced = 0;
        ptable[i].counter = 0;
    }

    /*
     * RAM, DISK memory initializations
     */
    RAM = malloc(sizeof(char*)*frames);
    DISK = malloc(sizeof(char*)*pages);

    for (int i = 0; i < frames; i++) {
        RAM[i] = malloc(sizeof(char)*PAGE_SIZE);
        strcpy(RAM[i], "");
    }

    for (int i = 0; i < pages; i++) {
        DISK[i] = malloc(sizeof(char)*PAGE_SIZE);
        char s[13];
        sprintf(s, "%s%d", "Somethin", i);
        strcpy(DISK[i], s);
    }

//    print_ptable();
    print_RAM();
    print_DISK();

    fflush(stdout);
    while (true) {
        pause();
    }
    return EXIT_SUCCESS;
}
