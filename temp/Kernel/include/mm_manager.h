#ifndef MM_MANAGER_H
#define MM_MANAGER_H

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef USE_BUDDY

    #define MIN_ORDER 5
    #define MAX_ORDER 16

    typedef struct BuddyBlock {
        struct BuddyBlock *next;
        int order;
    } BuddyBlock;

    typedef struct BuddyManager {
        char *base;
        size_t size;
        BuddyBlock *free_lists[MAX_ORDER + 1];
    } BuddyManager;

    typedef struct BuddyManager *MemoryManagerADT;

#else

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

#endif

typedef struct {
    uint64_t free, used, total;
} memoryData;

extern MemoryManagerADT globalMemoryManager;

//común para ambos managers
MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager,
                                     void *const restrict managedMemory,
                                     size_t managedSize);

void *allocMemory(MemoryManagerADT const restrict memoryManager,
                  const size_t memoryToAllocate);

int freeMemory(MemoryManagerADT const restrict memoryManager,
               void *const memoryToFree);

void getMemoryStatus(MemoryManagerADT const restrict memoryManager,
                     size_t *used, size_t *free);

void getMemoryData(memoryData *data);

#endif // MEMORY_MANAGER_H