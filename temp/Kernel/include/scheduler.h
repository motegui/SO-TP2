#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process_manager.h"

void schedule();//llama al scheduler manualmente
#define OFF 0
#define ON 1
void yield();// el proceso actual cede el CPU

PCB *pick_next_process();

void start_scheduler();

#endif