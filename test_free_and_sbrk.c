#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hmm.h"

#define MAX_ALLOCS 1000000

#define getInt(x, ...) atoi(x)

#define malloc(size) HmmAlloc(size)
#define free(ptr) HmmFree(ptr)

int main(int argc, char *argv[])
{
    char *ptr[MAX_ALLOCS];
    int freeStep, freeMin, freeMax, blockSize, numAllocs, j;

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        printf("%s num-allocs block-size [step [min [max]]]\n", argv[0]);
        exit(1);
    }

    numAllocs = getInt(argv[1]);
    if (numAllocs > MAX_ALLOCS) {
        printf("num-allocs > %d\n", MAX_ALLOCS);
        exit(1);
    }

    blockSize = getInt(argv[2]);

    freeStep = (argc > 3) ? getInt(argv[3]) : 1;
    freeMin =  (argc > 4) ? getInt(argv[4]) : 1;
    freeMax =  (argc > 5) ? getInt(argv[5]) : numAllocs;

    if (freeMax > numAllocs) {
        printf("free-max > num-allocs\n");
        exit(1);
    }

    printf("Allocating %d*%d bytes\n", numAllocs, blockSize);
    for (j = 0; j < numAllocs; j++) {
        ptr[j] = malloc(blockSize);
        if (ptr[j] == NULL) {
            printf("malloc returned null\n");
            exit(1);
        }
    }

    HmmPrintStats();

    printf("Freeing blocks from %d to %d in steps of %d\n",
                freeMin, freeMax, freeStep);
    for (j = freeMin - 1; j < freeMax; j += freeStep)
        free(ptr[j]);

    HmmPrintStats();

    exit(EXIT_SUCCESS);
}
