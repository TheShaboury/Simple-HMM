#ifndef HMM_H_
#define HMM_H_

#include <stddef.h>

void *HmmAlloc(size_t size);

void HmmFree(void *ptr);

void HmmPrintStats(void);

#endif
