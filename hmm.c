#include "hmm.h"
#include <stdio.h>
#include <string.h>

#define HEAP_SIZE (100 * 1024 * 1024)

#define MIN_HEAP_INCREMENT (4 * 1024)

typedef struct BlockHeader {
	size_t size;
	int is_free;
	struct BlockHeader *next;
} BlockHeader;

static char heap[HEAP_SIZE];

static char *program_break = heap;

static BlockHeader *free_list_head = NULL;

static size_t align_size(size_t size)
{
	return (size + 7) & ~7;
}

static BlockHeader *find_free_block(size_t size)
{
	BlockHeader *current = free_list_head;

	while (current != NULL)
	{
		if (current->is_free && current->size >= size)
		{
			return current;
		}
		current = current->next;
	}

	return NULL;
}

static BlockHeader *expand_heap(size_t size)
{
	size_t total_size = sizeof(BlockHeader) + size;

	size_t increment = total_size;
	if (increment < MIN_HEAP_INCREMENT)
	{
		increment = MIN_HEAP_INCREMENT;
	}

	if (program_break + increment > heap + HEAP_SIZE)
	{
		return NULL;
	}

	BlockHeader *block = (BlockHeader *)program_break;
	block->size = increment - sizeof(BlockHeader);
	block->is_free = 0;
	block->next = NULL;

	program_break += increment;

	if (free_list_head == NULL)
	{
		free_list_head = block;
	}
	else
	{
		BlockHeader *current = free_list_head;
		while (current->next != NULL)
		{
			current = current->next;
		}
		current->next = block;
	}

	return block;
}

static void shrink_heap(void)
{
	while (1)
	{
	BlockHeader *current = free_list_head;
	BlockHeader *prev = NULL;
	BlockHeader *last = NULL;
	BlockHeader *second_last = NULL;

	while (current != NULL)
	{
		second_last = prev;
		last = current;
		prev = current;
		current = current->next;
	}

	if (last != NULL && last->is_free)
	{
		size_t block_total_size = sizeof(BlockHeader) + last->size;

		program_break -= block_total_size;

		if (second_last != NULL)
		{
			second_last->next = NULL;
		}
		else
		{
			free_list_head = NULL;
		}
	}
	else
	{
		break;
	}
	}
}

void *HmmAlloc(size_t size)
{
	if (size == 0)
	{
		return NULL;
	}

	size = align_size(size);

	BlockHeader *block = find_free_block(size);

	if (block == NULL)
	{
		block = expand_heap(size);
		if (block == NULL)
		{
			return NULL;
		}
	}

	block->is_free = 0;

	return (void *)((char *)block + sizeof(BlockHeader));
}

void HmmFree(void *ptr)
{
	if (ptr == NULL)
	{
		return;
	}

	BlockHeader *block = (BlockHeader *)((char *)ptr - sizeof(BlockHeader));

	block->is_free = 1;

	shrink_heap();
}

void HmmPrintStats(void)
{
	size_t total_allocated = 0;
	size_t total_free = 0;
	int num_blocks = 0;

	BlockHeader *current = free_list_head;

	printf("\n=== Heap Statistics ===\n");
	printf("Heap base: %p\n", (void *)heap);
	printf("Program break: %p\n", (void *)program_break);
	printf("Heap used: %ld bytes\n", program_break - heap);

	while (current != NULL)
	{
		num_blocks++;
		if (current->is_free)
		{
			total_free += current->size;
		}
		else
		{
			total_allocated += current->size;
		}
		current = current->next;
	}

	printf("Number of blocks: %d\n", num_blocks);
	printf("Allocated: %ld bytes\n", total_allocated);
	printf("Free: %ld bytes\n", total_free);
	printf("====================\n\n");
}
