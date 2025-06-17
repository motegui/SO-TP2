#include "sync.h"
#include <string.h>
#include <videodriver.h>

#define MAX_SEMAPHORES 32

static Semaphore semaphores[MAX_SEMAPHORES];

static int get_free_slot() {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (!semaphores[i].in_use)
            return i;
    }
    return -1;
}

sem_t semCreate(int initValue) {
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

int semClose(sem_t sem) {
    if (sem == NULL || !sem->in_use)
        return -1;

    sem->in_use = false;
    return 0;
}

int semWait(sem_t sem) {
    printStringNColor("[SEM] wait 1\n", 24, (Color){255, 100, 255});

    if (sem == NULL || !sem->in_use)
        return -1;

    printStringNColor("[SEM] wait desp 1 if\n", 24, (Color){255, 100, 255});

    enter_region(&sem->lock);

    if (sem->value > 0) {
        sem->value--;
        leave_region(&sem->lock);
        return 0;
    }

    // No hay recursos: bloqueamos
    int pid = get_current_process()->pid;
    sem->blocked_pid = pid;
    get_current_process()->state = BLOCKED;

    leave_region(&sem->lock);

    printStringNColor("[SEM] desp block\n", 24, (Color){255, 100, 255});
    __asm__ volatile("int $0x20");

    return 0;
}


int semPost(sem_t sem) {

    if (sem == NULL || !sem->in_use)
        return -1;

    enter_region(&sem->lock);
    sem->value++;
    leave_region(&sem->lock);

    if (sem->blocked_pid != -1) {
        unblock_process(sem->blocked_pid);
        sem->blocked_pid = -1;
    }

    return 0;
}
