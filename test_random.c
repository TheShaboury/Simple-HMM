#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hmm.h"

#define MAX_ALLOCS 1000
#define MIN_BLOCK_SIZE 16
#define MAX_BLOCK_SIZE 4096

int main(int argc, char *argv[])
{
    void *ptrs[MAX_ALLOCS] = {0};
    int numAllocs = 100;
    
    if (argc > 1) {
        numAllocs = atoi(argv[1]);
        if (numAllocs > MAX_ALLOCS) {
            printf("num-allocs > %d\n", MAX_ALLOCS);
            exit(1);
        }
    }
    
    srand(time(NULL));
    
    printf("=== Random Allocation Test ===\n");
    printf("Performing %d random allocations\n\n", numAllocs);
    
    printf("Phase 1: Allocating blocks with random sizes...\n");
    for (int i = 0; i < numAllocs; i++) {
        size_t size = MIN_BLOCK_SIZE + (rand() % (MAX_BLOCK_SIZE - MIN_BLOCK_SIZE));
        ptrs[i] = HmmAlloc(size);
        
        if (ptrs[i] == NULL) {
            printf("Allocation %d failed (size=%zu)\n", i, size);
            exit(1);
        }
        
        memset(ptrs[i], i % 256, size);
    }
    printf("All allocations successful!\n");
    HmmPrintStats();
    
    printf("Phase 2: Freeing random blocks...\n");
    int numToFree = numAllocs / 2;
    for (int i = 0; i < numToFree; i++) {
        int idx = rand() % numAllocs;
        if (ptrs[idx] != NULL) {
            HmmFree(ptrs[idx]);
            ptrs[idx] = NULL;
        }
    }
    printf("Freed approximately %d blocks\n", numToFree);
    HmmPrintStats();
    
    printf("Phase 3: Allocating more blocks to test reuse...\n");
    for (int i = 0; i < numAllocs; i++) {
        if (ptrs[i] == NULL) {
            size_t size = MIN_BLOCK_SIZE + (rand() % (MAX_BLOCK_SIZE - MIN_BLOCK_SIZE));
            ptrs[i] = HmmAlloc(size);
            
            if (ptrs[i] != NULL) {
                memset(ptrs[i], i % 256, size);
            }
        }
    }
    HmmPrintStats();
    
    printf("Phase 4: Freeing all remaining blocks...\n");
    for (int i = 0; i < numAllocs; i++) {
        if (ptrs[i] != NULL) {
            HmmFree(ptrs[i]);
            ptrs[i] = NULL;
        }
    }
    HmmPrintStats();
    
    printf("Test completed successfully!\n");
    return 0;
}
