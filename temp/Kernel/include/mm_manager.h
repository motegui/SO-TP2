#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct Block {
	void *address;
	size_t size;
	bool is_free;
	struct Block *next;
} Block;

typedef struct MemoryManagerCDT {
	char *nextAddress;
	char *endAddress;
	Block *blockList;
} MemoryManagerCDT;

typedef struct MemoryManagerCDT *MemoryManagerADT;

extern MemoryManagerADT globalMemoryManager;


MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory, size_t managedSize);

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate);

int freeMemory(MemoryManagerADT const restrict memoryManager, void *const memoryToFree);

void getMemoryStatus(MemoryManagerADT const restrict memoryManager, size_t *used, size_t *free);

#endif
