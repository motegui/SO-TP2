#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <stdbool.h>

typedef enum
{
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} ProcessState;

typedef struct PCB
{
    int pid;
    int parent_pid;
    ProcessState state;
    char *name;
    int priority;
    bool foreground;
    int ticks;
    void *stack_base; //puntero a la base del stack
    void *stack_pointer; //puntero al stack actual; es el valor que el scheduler guarda y restaura en cada switch

} PCB;

#endif