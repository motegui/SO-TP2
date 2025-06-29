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
    if (memoryToAllocate == 0) return NULL;
    Block *curr = memoryManager->blockList;
    // Buscar bloque libre adecuado
    while (curr != NULL) {
        if (curr->is_free && curr->size >= memoryToAllocate) {
            curr->is_free = false;
            // Si el bloque es mucho más grande, partirlo
            if (curr->size >= memoryToAllocate + sizeof(Block) + 8) {
                Block *newBlock = (Block *)((char *)curr->address + memoryToAllocate);
                newBlock->address = (char *)curr->address + memoryToAllocate + sizeof(Block);
                newBlock->size = curr->size - memoryToAllocate - sizeof(Block);
                newBlock->is_free = true;
                newBlock->next = curr->next;
                curr->size = memoryToAllocate;
                curr->next = newBlock;
            }
            return curr->address;
        }
        curr = curr->next;
    }
    // No hay bloque libre, crear uno nuevo al final
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
    newBlock->next = NULL;
    // Insertar ordenado por dirección
    if (memoryManager->blockList == NULL || newBlock < memoryManager->blockList) {
        newBlock->next = memoryManager->blockList;
        memoryManager->blockList = newBlock;
    } else {
        Block *it = memoryManager->blockList;
        while (it->next && it->next < newBlock) it = it->next;
        newBlock->next = it->next;
        it->next = newBlock;
    }
    return allocation;
}

int freeMemory(MemoryManagerADT const restrict memoryManager, void *const memoryToFree) {
    if (!memoryToFree) return 1;
    Block *curr = memoryManager->blockList;
    Block *prev = NULL;
    while (curr != NULL) {
        if (curr->address == memoryToFree) {
            if (curr->is_free) return 2;
            curr->is_free = true;
            // Intentar fusionar con el siguiente
            if (curr->next && curr->next->is_free &&
                (char *)curr->address + curr->size == (char *)curr->next) {
                curr->size += sizeof(Block) + curr->next->size;
                curr->next = curr->next->next;
            }
            // Intentar fusionar con el anterior
            if (prev && prev->is_free &&
                (char *)prev->address + prev->size == (char *)curr) {
                prev->size += sizeof(Block) + curr->size;
                prev->next = curr->next;
            }
            return 0;
        }
        prev = curr;
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
