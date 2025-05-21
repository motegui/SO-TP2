#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory, size_t managedSize);

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate);

int freeMemory(MemoryManagerADT const restrict memoryManager, void *const memoryToFree);

void getMemoryStatus(MemoryManagerADT const restrict memoryManager, size_t *used, size_t *free);

#endif
