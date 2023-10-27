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

#define PAGE_SIZE 16

typedef struct PTE {
// The page is in the physical memory (RAM)
    bool valid;
// The frame number of the page in the RAM
    int frame;
// The page should be written to disk
    bool dirty;
// The page is referenced/requested
    int referenced;
} PTE;

PTE* ptable;
int pt;
char** RAM;
char** DISK;
int pages, frames;
int DISC_ACCESSES = 0;

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
        printf("Page %d ---> %s\n", i, DISK[i]);
    }
}

void print_RAM() {
    printf("RAM array\n");
    for (int i = 0; i < frames; i++) {
        printf("Frame %d ---> %s\n", i, RAM[i]);
    }
}

void swap(int signal) {
    int page_idx = -1;
    int mmu_pid = 0;
    for (int i = 0; i < pages; i++) {
        if (ptable[i].referenced != 0) {
            page_idx = i;  // found idx of page to be swapped into RAM !
            printf("Page %d is referenced\n", i);
        }
    }

    /*
     * If no request was found, stop, this signal is for termination
     */
    if (page_idx == -1) {
        printf("%d disk accesses in total\n", DISC_ACCESSES);
        close(pt);  // closing the pipe
        exit(EXIT_SUCCESS);
    }

    mmu_pid = ptable[page_idx].referenced;  // referenced contains pid of mmu
    int victim = -1;  // mean victim frame...
    printf("A disk access request from MMU Process (pid=%d)\n", mmu_pid);
    for (int RAM_frame = 0; RAM_frame < frames; RAM_frame++) {
        if (strcmp(RAM[RAM_frame], "") == 0) {  // found empty spot in RAM ?
            printf("We can allocate it to free frame %d\n", RAM_frame);
            printf("Copy data from the disk (page=%d) to RAM(frame=%d)", page_idx, RAM_frame);
            strcpy(RAM[RAM_frame], DISK[page_idx]);  // pull data from DISK into RAM
            victim = RAM_frame;
            break;
        }
    }
    if (victim == -1) {
        victim = rand() % frames;  // chosen a random frame, cause no free spots.
        printf("Choose a random victim frame %d\n", victim);
        int page_victim = 0;
        for (int i = 0; i < pages; i++) {  // try to find out what page is it
            if (ptable[i].frame == victim) {
                page_victim = i;
                ptable[i].frame = -1;
            }
        }
        printf("Replace/Evict it with page %d to be allocated to frame %d\n", page_victim, victim);

        if (ptable[page_victim].dirty) {
//            RAM[victim][rand() % (PAGE_SIZE - 3)] = 65 + (rand() % 20); // like writing into RAM.
            ptable[victim].dirty = 0;
            printf("Copy data from the disk %d to RAM frame %d\n", page_victim, victim);
//            strcpy(DISK[page_victim], RAM[victim]);
        }
    }
    /*
     * Updating the page table...
     */
    ptable[page_idx].frame = victim;
    ptable[page_idx].valid = true;
    ptable[page_idx].referenced = 0;
    ptable[page_idx].dirty = false;

    /*
     * Loading into memory...
     */
    strcpy(RAM[victim], DISK[page_idx]);

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
    if (argc != 3) {
        printf("Supply 2 args!\n");
        return EXIT_FAILURE;
    }
    srand(0);
    signal(SIGUSR1, swap);
    pages = atoi(argv[1]);
    frames = atoi(argv[2]);

    pt = open("pagetable", O_RDWR|O_CREAT, S_IRUSR | S_IWUSR);
    ftruncate(pt, sizeof(PTE) * pages);
    void *addr = mmap(NULL, sizeof(PTE)*pages, PROT_READ|PROT_WRITE, MAP_SHARED, pt, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        printf("errno: %d\n", errno);
        close(pt);
        return EXIT_FAILURE;
    }

    ptable = (PTE*)addr;
    for (int i = 0; i < pages; i++) {
        ptable[i].dirty = 0;
        ptable[i].frame = -1;
        ptable[i].valid = 0;
        ptable[i].referenced = 0;
    }

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
    print_ptable();
    print_RAM();
    print_DISK();

    printf("%d", getpid());
    fflush(stdout);
    while (true) {
        pause();
    }

    return EXIT_SUCCESS;
}
