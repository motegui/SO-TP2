#include <stdlib.h>
#include "include/scheduler.h"
#include "include/process_manager.h"

#define DEFAULT_QUANTUM 5

static int quantum_remaining = DEFAULT_QUANTUM;

PCB *pick_next_process();
void schedule();

void schedule() {
    PCB *current = get_current_process();

    if (current && current->state == RUNNING) {
        if (--quantum_remaining > 0)
            return;
        set_process_state(current, READY);
    }

    PCB *next = pick_next_process();
    if (!next) {
        next = get_idle_pcb(); // Devuelve el PCB del proceso idle
    }

    if (current && current != next)
        save_context(current);

    set_current_process(next);
    set_process_state(next, RUNNING);
    quantum_remaining = DEFAULT_QUANTUM;

    load_context(next);
}

PCB *pick_next_process() {
    PCB *chosen = NULL;
    int highest_priority = -1;

    static PCBNode *last_rr = NULL;
    PCBNode *start = last_rr ? last_rr->next : get_active_process_list();
    PCBNode *curr = start ? start : get_active_process_list(); // en caso de NULL

    do {
        if (curr && curr->pcb->state == READY) {
            if (curr->pcb->priority > highest_priority) {
                highest_priority = curr->pcb->priority;
                chosen = curr->pcb;
            } else if (curr->pcb->priority == highest_priority && !chosen) {
                chosen = curr->pcb;
            }
        }
        curr = curr->next ? curr->next : get_active_process_list();
    } while (curr != start);

    if (chosen) {
        last_rr = find_node_by_pid(chosen->pid);
    }

    return chosen;
}


void save_context(PCB *pcb) {
    __asm__ volatile("mov %%rsp, %0" : "=g"(pcb->stack_pointer));
}

void load_context(PCB *pcb) {
    __asm__ volatile("mov %0, %%rsp\n"
                 "ret"            // saltar a la dirección que estaba en la pila del proceso
                 :
                 : "g"(pcb->stack_pointer));
}
