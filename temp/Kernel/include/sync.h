#ifndef SYNC_H
#define SYNC_H

#include <stdbool.h>
#include <stdint.h>
#include "queue.h"
#include <process_manager.h>
#include <mm_manager.h>
#include <scheduler.h>

#define MAX_SEMAPHORES 128
#define SEM_NAME_LEN 32


typedef struct {
    int value;
    bool in_use;
    uint8_t lock;
    int blocked_pid; 
    Queue blocked_queue;
} Semaphore;

typedef Semaphore* sem_t;

sem_t sem_create(int initValue);
int sem_close(sem_t sem);
int sem_wait(sem_t sem);
int sem_post(sem_t sem);
void sem_unblock_all(sem_t sem);
void sem_remove_blocked_pid(int pid);
sem_t sem_open_named(uint64_t id, int initValue);
sem_t sem_get_named(uint64_t id);
int sem_wait_named(uint64_t id);
int sem_post_named(uint64_t id);
int sem_close_named(uint64_t id);
bool is_blocked_by_semaphore(int pid);

int enter_region(uint8_t *lock);
void leave_region(uint8_t *lock);

#endif
