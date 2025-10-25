# Heap Memory Manager (HMM) - Design Documentation

## Overview
This is a simple heap memory manager that simulates dynamic memory allocation in user space without kernel dependencies. It provides `HmmAlloc()` and `HmmFree()` functions similar to the standard `malloc()` and `free()`.

## Design Principles

### 1. Simulated Heap
- Uses a large static array (`heap[HEAP_SIZE]`) to simulate the heap area
- Size: 100 MB

### 2. Simulated Program Break
- A pointer variable (`program_break`) tracks the end of the used heap
- Initially points to the beginning of the heap array
- Moves forward when more memory is needed (simulating heap growth)
- Moves backward when memory can be released (simulating heap shrinkage)

### 3. Block Structure
Each memory block has a header containing:
```c
typedef struct BlockHeader {
    size_t size;           // Size of user data
    int is_free;           // 1 if free, 0 if allocated
    struct BlockHeader *next;  // Next block in the list
} BlockHeader;
```

### 4. Free List
- A linked list (`free_list_head`) tracks all blocks (both free and allocated)

## Memory Layout

```
+------------------+
|  Static Array    |  <- heap[0]
|  (Simulated      |
|   Heap)          |
+------------------+
| Block 1 Header   |
| Block 1 Data     |
+------------------+
| Block 2 Header   |
| Block 2 Data     |
+------------------+
| ...              |
+------------------+  <- program_break (end of used heap)
| Unused Space     |
+------------------+  <- heap[HEAP_SIZE] (end of array)
```

## Key Functions

### HmmAlloc(size_t size)
**Purpose:** Allocate memory of requested size

**Algorithm:**
1. Check if size is valid (> 0)
2. Align size to 8 bytes (for proper memory alignment)
3. Search free list for a free block that fits
4. If found: mark it as allocated and return pointer
5. If not found: expand heap by moving program_break forward
6. Return pointer to user data (after the header)

### HmmFree(void *ptr)
**Purpose:** Free previously allocated memory

**Algorithm:**
1. Check if pointer is valid (not NULL)
2. Get block header (located before user data pointer)
3. Mark block as free
4. Attempt to shrink heap if last block is free

## Flowcharts

### HmmAlloc() Flowchart
```
START
  ↓
[size == 0?] → YES → Return NULL
  ↓ NO
[Align size to 8 bytes]
  ↓
[Search free list for free block >= size]
  ↓
[Block found?] → YES → [Mark block allocated] → Return ptr
  ↓ NO                      ↑
[Calculate total size needed]
  ↓
[Round up to MIN_HEAP_INCREMENT]
  ↓
[Enough space in heap?] → NO → Return NULL
  ↓ YES
[Create block at program_break]
  ↓
[Move program_break forward]
  ↓
[Add block to free list]
  ↓
Return ptr to user data
  ↓
END
```

### HmmFree() Flowchart
```
START
  ↓
[ptr == NULL?] → YES → Return
  ↓ NO
[Get block header from ptr]
  ↓
[Mark block as free]
  ↓
[Find last block in list]
  ↓
[Is last block free?] → NO → Return
  ↓ YES
[Calculate block total size]
  ↓
[Move program_break backward]
  ↓
[Remove block from list]
  ↓
END
```

## Testing

### Test 1: test_free_and_sbrk.c
- Modified version of the provided test program
- Tests fixed-size allocations with configurable free patterns
- Usage: `./test_free_and_sbrk <num-allocs> <block-size> [step] [min] [max]`
- Example: `./test_free_and_sbrk 1000 1024 2 1 1000`

### Test 2: test_random.c
- Custom test with random allocation sizes
- Tests four phases:
  1. Random allocations (varying sizes)
  2. Random frees (approximately 50%)
  3. Re-allocations (tests block reuse)
  4. Free all (tests heap shrinking)
- Usage: `./test_random [num-allocs]`
- Example: `./test_random 100`

3. Better free list organization (e.g., size-based bins)
4. More aggressive heap shrinking strategies
5. Error checking and bounds validation
