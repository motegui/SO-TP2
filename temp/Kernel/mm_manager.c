#include "mm_manager.h"
#include <string.h>
#include <stdint.h>

MemoryManagerADT globalMemoryManager = NULL;
static uint64_t total_memory = 0;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory, size_t managedSize) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
	memoryManager->nextAddress = (char *) managedMemory;
	memoryManager->endAddress = memoryManager->nextAddress + managedSize;
	memoryManager->blockList = NULL;
	total_memory = managedSize;
	return memoryManager;
}

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate) {
    Block *curr = memoryManager->blockList;
    while (curr != NULL) {
        if (curr->is_free && curr->size >= memoryToAllocate) {
            curr->is_free = false;
            return curr->address;
        }
        curr = curr->next;
    }

    size_t totalSize = memoryToAllocate + sizeof(Block);
    if (memoryManager->nextAddress + totalSize > memoryManager->endAddress)
        return NULL;

    Block *newBlock = (Block *) memoryManager->nextAddress;
    memoryManager->nextAddress += sizeof(Block);

    void *allocation = memoryManager->nextAddress;
    memoryManager->nextAddress += memoryToAllocate;

    newBlock->address = allocation;
    newBlock->size = memoryToAllocate;
    newBlock->is_free = false;
    newBlock->next = memoryManager->blockList;
    memoryManager->blockList = newBlock;

    return allocation;
}



int freeMemory(MemoryManagerADT const restrict memoryManager, void *const memoryToFree) {
    Block *curr = memoryManager->blockList;
    while (curr != NULL) {
        if (curr->address == memoryToFree) {
            if (curr->is_free)
                return 2;
            curr->is_free = true;

            Block *scan = memoryManager->blockList;
            while (scan != NULL) {
                if (scan != curr && scan->is_free && 
                    (char*)scan->address + scan->size == (char*)curr->address) {
                    scan->size += sizeof(Block) + curr->size;
                    scan->next = curr->next;
                    curr = scan;
                }
                scan = scan->next;
            }

            return 0;
        }
        curr = curr->next;
    }
    return 1;
}

void getMemoryStatus(MemoryManagerADT const restrict memoryManager, size_t *used, size_t *free) {
	*used = 0;
	*free = 0;

	Block *curr = memoryManager->blockList;
	while (curr != NULL) {
		if (curr->is_free)
			*free += curr->size;
		else
			*used += curr->size;
		curr = curr->next;
	}
}

void getMemoryData(memoryData *data) {
    size_t used, free;
    getMemoryStatus(globalMemoryManager, &used, &free);
    data->used = used;
    data->free = free;
    data->total = total_memory;
}
