#include "sync.h"
#include <string.h>

static Semaphore semaphores[MAX_SEMAPHORES];

static int find_semaphore_index(const char *name) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i].in_use && strcmp(semaphores[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static int get_free_slot() {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (!semaphores[i].in_use) {
            return i;
        }
    }
    return -1;
}

sem_t semCreate(char *name, int initValue) {
    int existing = find_semaphore_index(name);
    if (existing != -1) {
        semaphores[existing].ref_count++;
        return &semaphores[existing];
    }

    int index = get_free_slot();
    if (index == -1) return NULL;

    Semaphore *sem = &semaphores[index];

    strncpy(sem->name, name, SEM_NAME_LEN - 1);
    sem->name[SEM_NAME_LEN - 1] = '\0';

    sem->value = initValue;
    sem->ref_count = 1;
    sem->in_use = true;
    queue_init(&sem->waiting_queue);

    return sem;
}

sem_t semOpen(const char *name) {
    int index = find_semaphore_index(name);

    if (index == -1) {
        return NULL;
    }

    semaphores[index].ref_count++;
    return &semaphores[index];
}

int semClose(sem_t sem) {
    if (sem == NULL || !sem->in_use)
        return -1;

    sem->ref_count--;

    if (sem->ref_count <= 0) {
        sem->in_use = false;
        queue_clear(&sem->waiting_queue);
    }

    return 0;
}

int semWait(sem_t sem) {
    return 0;
}


int semPost(sem_t sem){
    return 0;
}