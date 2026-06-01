#include "sync.h"
#include <string.h>
#include <videodriver.h>
#include "stdbool.h"

static Semaphore semaphores[MAX_SEMAPHORES];

typedef struct {
    uint64_t id;
    sem_t sem;
    int in_use;
} NamedSemaphore;

static NamedSemaphore named_sems[MAX_SEMAPHORES];

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

    sem_unblock_all(sem);
    sem->in_use = false;
    queue_clear(&sem->blocked_queue);
    return 0;
}

void sem_unblock_all(sem_t sem) {
    if (sem == NULL || !sem->in_use) {
        return;
    }

    enter_region(&sem->lock);
    while (!queue_is_empty(&sem->blocked_queue)) {
        int pid = queue_dequeue(&sem->blocked_queue);
        unblock_process(pid);
    }
    leave_region(&sem->lock);
}

void sem_remove_blocked_pid(int pid) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        Semaphore *sem = &semaphores[i];
        if (!sem->in_use) {
            continue;
        }

        enter_region(&sem->lock);
        QueueNode *prev = NULL;
        QueueNode *curr = sem->blocked_queue.front;
        while (curr != NULL) {
            if (curr->pid == pid) {
                QueueNode *to_delete = curr;
                curr = curr->next;
                if (prev == NULL) {
                    sem->blocked_queue.front = curr;
                } else {
                    prev->next = curr;
                }
                if (sem->blocked_queue.rear == to_delete) {
                    sem->blocked_queue.rear = prev;
                }
                freeMemory(globalMemoryManager, to_delete);
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
        leave_region(&sem->lock);
    }
}

static int queue_contains_pid(Queue *q, int pid) {
    QueueNode *curr = q->front;
    while (curr) {
        if (curr->pid == pid) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

int sem_wait(sem_t sem) {
    if (sem == NULL || !sem->in_use) {
        return -1;
    }

    while (1) {
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

        if (sem == NULL || !sem->in_use) {
            return -1;
        }

        enter_region(&sem->lock);
        int still_waiting = queue_contains_pid(&sem->blocked_queue, pid);
        leave_region(&sem->lock);
        if (!still_waiting) {
            return 0;
        }
    }
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

sem_t sem_open_named(uint64_t id, int initValue) {
    sem_t existing = sem_get_named(id);
    if (existing != NULL) {
        return existing;
    }

    sem_t sem = sem_create(initValue);
    if (sem == NULL) {
        return NULL;
    }

    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (!named_sems[i].in_use) {
            named_sems[i].id = id;
            named_sems[i].sem = sem;
            named_sems[i].in_use = 1;
            return sem;
        }
    }

    sem_close(sem);
    return NULL;
}

sem_t sem_get_named(uint64_t id) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (named_sems[i].in_use && named_sems[i].id == id) {
            return named_sems[i].sem;
        }
    }
    return NULL;
}

int sem_wait_named(uint64_t id) {
    sem_t sem = sem_get_named(id);
    if (sem == NULL) {
        return -1;
    }
    return sem_wait(sem);
}

int sem_post_named(uint64_t id) {
    sem_t sem = sem_get_named(id);
    if (sem == NULL) {
        return -1;
    }
    return sem_post(sem);
}

int sem_close_named(uint64_t id) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (named_sems[i].in_use && named_sems[i].id == id) {
            sem_close(named_sems[i].sem);
            named_sems[i].in_use = 0;
            named_sems[i].sem = NULL;
            return 0;
        }
    }
    return -1;
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
