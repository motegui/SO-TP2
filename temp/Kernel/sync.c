#include "sync.h"
#include <string.h>
#include <videodriver.h>
#include "stdbool.h"

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
    queue_init(&sem->blocked_queue);

    return sem;
}

int sem_close(sem_t sem) {
    if (sem == NULL || !sem->in_use)
        return -1;

    sem->in_use = false;
    queue_clear(&sem->blocked_queue);
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

    int pid = get_current_process()->pid;
    queue_enqueue(&sem->blocked_queue, pid);
    leave_region(&sem->lock);
    __asm__ volatile("int $0x20");
    return 0;
}



int sem_post(sem_t sem) {
    if (sem == NULL || !sem->in_use)
        return -1;

    enter_region(&sem->lock);
    if (!queue_is_empty(&sem->blocked_queue)) {
        int pid = queue_dequeue(&sem->blocked_queue);
        leave_region(&sem->lock);
        unblock_process(pid);
        return 0;
    }

    sem->value++;
    leave_region(&sem->lock);
    return 0;
}

bool is_blocked_by_semaphore(int pid) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (!semaphores[i].in_use) continue;
        QueueNode *curr = semaphores[i].blocked_queue.front;
        while (curr) {
            if (curr->pid == pid) {
                return true;
            }
            curr = curr->next;
        }
    }
    return false;
}