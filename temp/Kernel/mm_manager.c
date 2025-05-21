#include "mm_manager.h"
#include <string.h>

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

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory, size_t managedSize) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
	memoryManager->nextAddress = (char *) managedMemory;
	memoryManager->endAddress = memoryManager->nextAddress + managedSize;
	memoryManager->blockList = NULL;
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

	if (memoryManager->nextAddress + memoryToAllocate > memoryManager->endAddress)
		return NULL; 

	void *allocation = memoryManager->nextAddress;
	memoryManager->nextAddress += memoryToAllocate;

	Block *newBlock = (Block *) memoryManager->nextAddress;
	memoryManager->nextAddress += sizeof(Block);

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
