#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
// Forward declaration to avoid circular dependency
struct PCB;
typedef struct PCB PCB;

uint64_t schedule(uint64_t current_rsp); //llama al scheduler manualmente

void yield();// el proceso actual cede el CPU

PCB *pick_next_process();

void load_context(PCB *pcb) __attribute__((noreturn));
int get_foreground_pid();

#endif