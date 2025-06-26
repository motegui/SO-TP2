#include "sync.h"
#include <string.h>
#include <videodriver.h>
#include "stdbool.h"

#define MAX_SEMAPHORES 32

static Semaphore semaphores[MAX_SEMAPHORES];

static int get_free_slot() {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (!semaphores[i].in_use)
            return i;
    }
    return -1;
}

sem_t sem_create(int initValue) {
    int index = get_free_slot();
    if (index == -1)
        return NULL;

    Semaphore *sem = &semaphores[index];
    sem->value = initValue;
    sem->in_use = true;
    sem->lock = 0;
    sem->blocked_pid = -1;

    return sem;
}

int sem_close(sem_t sem) {
    if (sem == NULL || !sem->in_use)
        return -1;

    sem->in_use = false;
    return 0;
}

int sem_wait(sem_t sem) {
    if (sem == NULL || !sem->in_use)
        return -1;

    enter_region(&sem->lock);
    if (sem->value > 0) {
        sem->value--;
        leave_region(&sem->lock);
        return 0;
    }
    // Bloqueamos el proceso actual
    sem->blocked_pid = get_current_process()->pid;
    //get_current_process()->state = BLOCKED;
    leave_region(&sem->lock);
    __asm__ volatile("int $0x20");
    return 0;
}



int sem_post(sem_t sem) {
    int to_unblock = sem->blocked_pid;

    if (sem == NULL || !sem->in_use)
        return -1;

    enter_region(&sem->lock);
    sem->value++;
    leave_region(&sem->lock);

    if (sem->blocked_pid != -1) {
        unblock_process(to_unblock);
        sem->blocked_pid = -1;
    }

    return 0;
}

bool is_blocked_by_semaphore(int pid) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i].in_use && semaphores[i].blocked_pid == pid) {
            return true;
        }
    }
    return false;
}