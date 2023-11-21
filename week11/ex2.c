#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BLOCK_SIZE 1024
#define FILENAME_SIZE 16
#define BLOCKS_PER_FILE 8
#define N_FILES 16
#define N_BLOCKS 128
#define USED 1
#define UNUSED 0

typedef struct __attribute__((__packed__)) {
    char name[FILENAME_SIZE]; // file name 16
    int size; // file size (in number of blocks) 4 20
    int blockPointers[BLOCKS_PER_FILE]; // direct block pointers 4 * 8 = 32 52
    int used; // 0 => inode is free; 1 => in use
} inode;

//inode* inode_list;
//char* fb_list;  // free block list
char* disk_name;
int op_idx = 0;

FILE* fs;

void parse_free_block(char* buffer) {
    fseek(fs, 0, SEEK_SET);
    fread(buffer, sizeof(char), N_BLOCKS, fs);
}

void parse_inode_block(inode* buffer) {
    fseek(fs, N_BLOCKS, SEEK_SET);
    for (int i = 0; i < N_FILES; i++) {
        fread(&buffer[i], sizeof(inode), 1, fs);
    }
}

void commit_free_block(char* buffer) {
    fseek(fs, 0, SEEK_SET);
    fwrite(buffer, sizeof(char), N_BLOCKS, fs);
}

void commit_inode_block(inode* buffer) {
    fseek(fs, N_BLOCKS, SEEK_SET);
    fwrite(buffer, sizeof(inode), N_FILES, fs);
}

void perform_reading(char buf[BLOCK_SIZE], int address) {
    fseek(fs, address * BLOCK_SIZE, SEEK_SET);
    fscanf(fs, "%s", buf);
}

void perform_writing(const char buf[BLOCK_SIZE], int address) {
    fseek(fs, address * BLOCK_SIZE, SEEK_SET);
    fprintf(fs, "Write operation %d\n", op_idx);
}

int locate_inode_by_name(const inode* inode_list, const char* name) {
    int idx = -1;
    for (int i = 0; i < N_FILES; i++) {
        if (strcmp(name, inode_list[i].name) == 0) {
            idx = i;
            break;
        }
    }
    return idx;
}

int create(char name[FILENAME_SIZE], int size){
    /* create a file with this name and this size */

    int pntrs[BLOCKS_PER_FILE];
    int cur = 0;
    char fb_list[N_BLOCKS];
    memset(fb_list, 0, N_BLOCKS);

    parse_free_block(fb_list);
    for (int block = 0; block < N_BLOCKS; block++) {
        if (fb_list[block] == UNUSED) {
            pntrs[cur] = block;
            cur++;
        }
        if (cur == size) {
            break;
        }
    }

    if (cur != size) {
       printf("No free space !\n");
       return EXIT_FAILURE;
   }

    inode inode_list[N_FILES];
    parse_inode_block(inode_list);

    int free_inode = -1;
    for (int idx = 0; idx < N_FILES; idx++) {
        if (!inode_list[idx].used) {
            free_inode = idx;
            break;
        }
    }

    inode_list[free_inode].used = USED;
    inode_list[free_inode].size = size;
    strcpy(inode_list[free_inode].name, name);

    for (int i = 0; i < size; i++) {
        fb_list[pntrs[i]] = USED;
        inode_list[free_inode].blockPointers[i] = pntrs[i];
    }

    commit_free_block(fb_list);
    commit_inode_block(inode_list);

} // End Create


int delete(char name[FILENAME_SIZE]){
    // Delete the file with this name

    char fb_list[N_BLOCKS];
    memset(fb_list, 0, N_BLOCKS);

    inode inode_list[N_FILES];

    parse_free_block(fb_list);
    parse_inode_block(inode_list);

    int idx = locate_inode_by_name(inode_list, name);

    if (idx == -1) {
        printf("No such file to delete !\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < inode_list[idx].size; i++) {
        fb_list[inode_list[idx].blockPointers[i]] = UNUSED;
    }

    // Step 3: mark inode as free
    // Set the "used" field to 0.
    // and also erase name.
    inode_list[idx].used = UNUSED;
    strcpy(inode_list[idx].name, "");


    commit_free_block(fb_list);
    commit_inode_block(inode_list);

} // End Delete


int ls(void){
    // List names of all files on disk

    inode inode_list[N_FILES];

    parse_inode_block(inode_list);
    for (int idx = 0; idx < N_FILES; idx++) {
        if (inode_list[idx].used) {
            printf("%s - %d\n", inode_list[idx].name, inode_list[idx].size);
        }
    }

} // End ls

int read_(char name[FILENAME_SIZE], int blockNum, char buf[BLOCK_SIZE]){

    inode inode_list[N_FILES];

    parse_inode_block(inode_list);
    int idx = locate_inode_by_name(inode_list, name);

    if (idx == -1) {
        printf("No such file to read !\n");
        return EXIT_FAILURE;
    }

    if (blockNum >= inode_list[idx].size) {
        printf("Block is too big to belong to a file !\n");
        return EXIT_FAILURE;
    }

    int address = inode_list[idx].blockPointers[blockNum];
    // Read in the block! => Read in 1024 bytes from this location into the buffer "buf"
    perform_reading(buf, address);

} // End read


int write_(char name[FILENAME_SIZE], int blockNum, char buf[BLOCK_SIZE]){

    inode inode_list[N_FILES];

    parse_inode_block(inode_list);
    int idx = locate_inode_by_name(inode_list, name);

    if (idx == -1) {
        printf("No such file to write to !\n");
        return EXIT_FAILURE;
    }

    if (blockNum >= inode_list[idx].size) {
        printf("Block is too big to belong to a file !\n");
        return EXIT_FAILURE;
    }

    int address = inode_list[idx].blockPointers[blockNum];
    // Write the block! => Write 1024 bytes from the buffer "buff" to this location
    perform_writing(buf, address);

} // end write

int main(int argc, char *argv[]){
    if (argc < 2) {
        printf("usage: %s <sequence of operations>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    disk_name = argv[1];
    fs = fopen(disk_name, "r+");

    char buffer[BLOCK_SIZE];
    for (int idx = 2; idx < argc; idx++) {
        op_idx++;
        if (strcmp("C", argv[idx]) == 0) {  // request to create a file
            /*
             * then [idx + 1] is a pointer to the filename, and [idx + 2] is size.
             */
            char* filename = argv[idx + 1];
            int size = atoi(argv[idx + 2]);

            create(filename, size);
            idx += 2; // so we do not perceive filename and size as an action.
        }

        if (strcmp("R", argv[idx]) == 0) {  // request to read a file
            /*
             * then [idx + 1] is a pointer to the filename, and [idx + 2] is block id.
             */
            char* filename = argv[idx + 1];
            int block = atoi(argv[idx + 2]);

            read_(filename, block, buffer);
            idx += 2; // so we do not perceive filename and block as an action.
        }

        if (strcmp("W", argv[idx]) == 0) {  // request to write to a file
            /*
             * then [idx + 1] is a pointer to the filename, and [idx + 2] is block id.
             */
            char* filename = argv[idx + 1];
            int block = atoi(argv[idx + 2]);

            write_(filename, block, buffer);
            idx += 2; // so we do not perceive filename and block as an action.
        }

        if (strcmp("D", argv[idx]) == 0) {
            /*
             * Then [idx + 1] id a pointer to the filename
             */
            char* filename = argv[idx + 1];

            delete(filename);
            idx++;
        }

        if (strcmp("L", argv[idx]) == 0) {  // have to list all files on the disk.
            ls();
        }
    }

    fclose(fs);

    return EXIT_SUCCESS;
}
