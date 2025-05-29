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
    static PCBNode *last_rr = NULL;
    PCBNode *start = last_rr ? last_rr->next : get_active_process_list();
    if (!start) return NULL;

    PCBNode *curr = start;
    int max_priority = -1;

    // PASO 1: Buscar la prioridad más alta entre procesos READY
    do {
        if (curr->pcb->state == READY && curr->pcb->priority > max_priority) {
            max_priority = curr->pcb->priority;
        }
        curr = curr->next ? curr->next : get_active_process_list();
    } while (curr != start);

    // PASO 2: Buscar desde last_rr->next al siguiente proceso READY con esa prioridad
    curr = start;
    do {
        if (curr->pcb->state == READY && curr->pcb->priority == max_priority) {
            last_rr = curr;               // Actualizamos el último elegido
            return curr->pcb;             // Y lo devolvemos
        }
        curr = curr->next ? curr->next : get_active_process_list();
    } while (curr != start);

    return NULL;  // Si no hay READY, devolvemos NULL
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
