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

typedef struct inode {
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
    printf("%ld\n", ftell(fs));
    fread(buffer, sizeof(char), N_BLOCKS, fs);
}

void parse_inode_block(inode* buffer) {
    fseek(fs, N_BLOCKS, SEEK_SET);
    printf("%ld\n", ftell(fs));
    for (int i = 0; i < N_FILES; i++) {
        fread(&buffer[i].name, sizeof(char), FILENAME_SIZE, fs);
        fscanf(fs, "%d", &buffer[i].size);
        for (int b = 0; b < BLOCKS_PER_FILE; b++) {
            fscanf(fs, "%d", &buffer[i].blockPointers[b]);
        }
        fscanf(fs, "%d", &buffer[i].used);
    }
}

void commit_free_block(char* buffer) {
    fseek(fs, 0, SEEK_SET);
    for (int block = 0; block < N_BLOCKS; block++) {
        fprintf(fs, "%c", buffer[block]);
    }
}

void commit_inode_block(inode* buffer) {
    fseek(fs, N_BLOCKS, SEEK_SET);
    for (int i = 0; i < N_FILES; i++) {
        fprintf(fs, "%s", buffer[i].name);
        // if we got less than 16 bytes string, then just append zeros to the end to fulfill the length.
        for (int i = 0; i < FILENAME_SIZE - 1 - strlen(buffer[i].name); i++) {
            fprintf(fs, "%c", 0);
        }

        fprintf(fs, "%d", buffer[i].size);
        for (int b = 0; b < BLOCKS_PER_FILE; b++) {
            fprintf(fs, "%d", buffer[i].blockPointers[b]);
        }
        fprintf(fs, "%d", buffer[i].used);
    }
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

    // Step 1: check to see if we have sufficient free space on disk by
    // reading in the free block list. To do this:
    // move the file pointer to the start of the disk file.
    // Read the first 128 bytes (the free/in-use block information)
    // Scan the list to make sure you have sufficient free blocks to
    // allocate a new file of this size
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

    // Step 2: we look  for a free inode on disk
    // Read in an inode
    // check the "used" field to see if it is free
    // If not, repeat the above two steps until you find a free inode
    // Set the "used" field to 1
    // Copy the filename to the "name" field,
    // Copy the file size (in units of blocks) to the "size" field
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

    // Step 3: Allocate data blocks to the file
    // for(i=0;i<size;i++)
    // Scan the block list that you read in Step 1 for a free block
    // Once you find a free block, mark it as in-use (Set it to 1)
    // Set the blockPointer[i] field in the inode to this block number.
    //
    // end for
    for (int i = 0; i < size; i++) {
        fb_list[pntrs[i]] = USED;
        inode_list[free_inode].blockPointers[i] = pntrs[i];
    }


    // Step 4: Write out the inode and free block list to disk
    // Move the file pointer to the start of the file
    // Write out the 128 byte free block list
    // Move the file pointer to the position on disk where this inode was stored
    // Write out the inode
    commit_free_block(fb_list);
    commit_inode_block(inode_list);

} // End Create


int delete(char name[FILENAME_SIZE]){
    // Delete the file with this name

    // Step 1: Locate the inode for this file
    // Move the file pointer to the 1st inode (129th byte)
    // Read in a inode
    // If the inode is free, repeat above step.
    // If the inode is in use, check if the "name" field in the
    // inode matches the file we want to delete. IF not, read the next
    // inode and repeat
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

    // Step 2: free blocks of the file being deleted
    // Read in the 128 byte free block list (move file pointer to start of the disk and read in 128 bytes)
    // Free each block listed in the blockPointer fields as follows:
    // for(i=0;i< inode.size; i++)
    // freeblockList[ inode.blockPointer[i] ] = 0;
    for (int i = 0; i < inode_list[idx].size; i++) {
        fb_list[inode_list[idx].blockPointers[i]] = UNUSED;
    }

    // Step 3: mark inode as free
    // Set the "used" field to 0.
    // and also erase name.
    inode_list[idx].used = UNUSED;
    strcpy(inode_list[idx].name, "");


    // Step 4: Write out the inode and free block list to disk
    // Move the file pointer to the start of the file
    // Write out the 128 byte free block list
    // Move the file pointer to the position on disk where this inode was stored
    // Write out the inode
    commit_free_block(fb_list);
    commit_inode_block(inode_list);

} // End Delete


int ls(void){
    // List names of all files on disk

    // Step 1: read in each inode and print!
    // Move file pointer to the position of the 1st inode (129th byte)
    // for(i=0;i<16;i++)
    // REad in a inode
    // If the inode is in-use
    // print the "name" and "size" fields from the inode
    inode inode_list[N_FILES];

    parse_inode_block(inode_list);
    for (int idx = 0; idx < N_FILES; idx++) {
        if (inode_list[idx].used) {
            printf("%s - %d\n", inode_list[idx].name, inode_list[idx].size);
        }
    }

} // End ls

int read_(char name[FILENAME_SIZE], int blockNum, char buf[BLOCK_SIZE]){

    // read this block from this file

    // Step 1: locate the inode for this file
    // Move file pointer to the position of the 1st inode (129th byte)
    // Read in a inode
    // If the inode is in use, compare the "name" field with the above file
    // IF the file names don't match, repeat

    inode inode_list[N_FILES];

    parse_inode_block(inode_list);
    int idx = locate_inode_by_name(inode_list, name);

    if (idx == -1) {
        printf("No such file to read !\n");
        return EXIT_FAILURE;
    }

    // Step 2: Read in the specified block
    // Check that blockNum < inode.size, else flag an error
    // Get the disk address of the specified block
    // That is, addr = inode.blockPointer[blockNum]
    // move the file pointer to the block location (i.e., to byte # addr*1024 in the file)
    if (blockNum >= inode_list[idx].size) {
        printf("Block is too big to belong to a file !\n");
        return EXIT_FAILURE;
    }

    int address = inode_list[idx].blockPointers[blockNum];
    // Read in the block! => Read in 1024 bytes from this location into the buffer "buf"
    perform_reading(buf, address);

} // End read


int write_(char name[FILENAME_SIZE], int blockNum, char buf[BLOCK_SIZE]){

    // write this block to this file

    // Step 1: locate the inode for this file
    // Move file pointer to the position of the 1st inode (129th byte)
    // Read in a inode
    // If the inode is in use, compare the "name" field with the above file
    // IF the file names don't match, repeat
    inode inode_list[N_FILES];

    parse_inode_block(inode_list);
    int idx = locate_inode_by_name(inode_list, name);

    if (idx == -1) {
        printf("No such file to write to !\n");
        return EXIT_FAILURE;
    }

    // Step 2: Write to the specified block
    // Check that blockNum < inode.size, else flag an error
    // Get the disk address of the specified block
    // That is, addr = inode.blockPointer[blockNum]
    // move the file pointer to the block location (i.e., byte # addr*1024)
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
    fprintf(fs, "%c", 1);

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
