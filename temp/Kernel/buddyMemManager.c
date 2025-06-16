#ifdef USE_BUDDY
#include "mm_manager.h"
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define BUDDY_ALIGNMENT (1UL << MIN_ORDER)

MemoryManagerADT globalMemoryManager = NULL;

static int is_aligned(void *ptr) {
    return ((uintptr_t)ptr & (BUDDY_ALIGNMENT - 1)) == 0;
}

static int get_order(size_t size) {
    if (size == 0) return -1;
    
    size_t required_size = size + sizeof(BuddyBlock);
    size_t min_block = (1UL << MIN_ORDER);
    if (required_size < min_block) return MIN_ORDER;
    
    int order = MIN_ORDER;
    size_t block_size = min_block;
    
    while (block_size < required_size && order < MAX_ORDER) {
        order++;
        block_size <<= 1;
    }
    
    return order;
}

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager,
                                   void *const restrict managedMemory,
                                   size_t managedSize) {
    if (!memoryForMemoryManager || !managedMemory || 
        managedSize < (1UL << MIN_ORDER) || !is_aligned(managedMemory)) {
        return NULL;
    }

    BuddyManager *manager = (BuddyManager *)memoryForMemoryManager;
    manager->base = (char *)managedMemory;
    manager->size = managedSize;
    memset(manager->free_lists, 0, sizeof(manager->free_lists));

    // Find maximum possible order that fits in managedSize
    int order = MAX_ORDER;
    while ((1UL << order) > managedSize && order > MIN_ORDER) {
        order--;
    }

    // Initialize the single free block
    BuddyBlock *initialBlock = (BuddyBlock *)manager->base;
    initialBlock->next = NULL;
    initialBlock->order = order;
    manager->free_lists[order] = initialBlock;

    return manager;
}

void *allocMemory(MemoryManagerADT const restrict memoryManager,
                 const size_t memoryToAllocate) {
    if (!memoryManager || memoryToAllocate == 0) return NULL;
    
    BuddyManager *manager = (BuddyManager *)memoryManager;
    int order = get_order(memoryToAllocate);
    if (order < MIN_ORDER || order > MAX_ORDER) return NULL;

    // Find the first available order with free blocks
    int current = order;
    while (current <= MAX_ORDER && manager->free_lists[current] == NULL) {
        current++;
    }

    if (current > MAX_ORDER) {
        return NULL; // No memory available
    }

    // Split blocks until we reach the desired order
    while (current > order) {
        BuddyBlock *block = manager->free_lists[current];
        manager->free_lists[current] = block->next;

        current--;
        size_t blockSize = 1UL << current;
        
        BuddyBlock *buddy = (BuddyBlock *)((char *)block + blockSize);
        buddy->order = current;
        buddy->next = manager->free_lists[current];
        
        block->order = current;
        block->next = buddy; // Keep blocks together
        
        manager->free_lists[current] = block;
    }

    // Allocate the block
    BuddyBlock *allocated = manager->free_lists[order];
    manager->free_lists[order] = allocated->next;
    
    return (char *)allocated + sizeof(BuddyBlock);
}

static BuddyBlock *find_and_remove_buddy(BuddyManager *manager, BuddyBlock *block, int order) {
    BuddyBlock **curr = &manager->free_lists[order];
    while (*curr) {
        if (*curr == block && (*curr)->order == order) {
            BuddyBlock *found = *curr;
            *curr = found->next;
            return found;
        }
        curr = &(*curr)->next;
    }
    return NULL;
}

int freeMemory(MemoryManagerADT const restrict memoryManager,
              void *const memoryToFree) {
    if (!memoryManager || !memoryToFree) return -1;
    
    BuddyManager *manager = (BuddyManager *)memoryManager;
    BuddyBlock *block = (BuddyBlock *)((char *)memoryToFree - sizeof(BuddyBlock));
    
    // Basic validation
    if ((char *)block < manager->base || 
        (char *)block >= manager->base + manager->size) {
        return -1;
    }
    
    char *base = manager->base;
    size_t offset = (char *)block - base;
    int order = block->order;
    
    if (order < MIN_ORDER || order > MAX_ORDER) return -1;

    // Coalesce with buddies
    while (order < MAX_ORDER) {
        size_t buddy_offset = offset ^ (1UL << order);
        if (buddy_offset >= manager->size) break;
        
        BuddyBlock *buddy = (BuddyBlock *)(base + buddy_offset);
        
        // Try to remove buddy from free list
        if (!find_and_remove_buddy(manager, buddy, order)) break;
        
        // Merge blocks (keep the lower address)
        if (block > buddy) {
            BuddyBlock *tmp = block;
            block = buddy;
            buddy = tmp;
            offset = buddy_offset;
        }
        
        order++;
        block->order = order;
    }
    
    // Add to free list
    block->next = manager->free_lists[order];
    manager->free_lists[order] = block;
    return 0;
}

void getMemoryStatus(MemoryManagerADT const restrict memoryManager,
                   size_t *used, size_t *free) {
    if (!memoryManager || !used || !free) return;
    
    BuddyManager *manager = (BuddyManager *)memoryManager;
    *free = 0;
    for (int i = MIN_ORDER; i <= MAX_ORDER; i++) {
        BuddyBlock *curr = manager->free_lists[i];
        while (curr != NULL) {
            *free += (1UL << i);
            curr = curr->next;
        }
    }
    *used = manager->size - *free;
}

#endif