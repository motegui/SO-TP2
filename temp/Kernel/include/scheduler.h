#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process_manager.h"


uint64_t schedule(uint64_t current_rsp); //llama al scheduler manualmente

void yield();// el proceso actual cede el CPU

PCB *pick_next_process();

void load_context(PCB *pcb) __attribute__((noreturn));

#endif