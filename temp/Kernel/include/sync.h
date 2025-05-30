#ifndef SYNC_H
#define SYNC_H

#include <stdbool.h>
#include <stdint.h>
#include "queue.h"
#include <process_manager.h>
#include <mm_manager.h>
#include <scheduler.h>

#define MAX_SEMAPHORES 64
#define SEM_NAME_LEN 32

typedef struct {
    char name[SEM_NAME_LEN];
    int value;
    int ref_count; //cantidad de procesos que estan usando el semaforo
    Queue waiting_queue; //cola de procesos bloqueados esperando 
    bool in_use;
} Semaphore;

typedef Semaphore* sem_t;

sem_t semCreate(char *name, int initValue);
sem_t semOpen(const char *name);
int semClose(sem_t sem);
int semWait(sem_t sem);
int semPost(sem_t sem);
#endif