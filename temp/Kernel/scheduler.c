#include "scheduler.h"
#include "process_manager.h"

#define DEFAULT_QUANTUM 5

static int quantum_remaining = DEFAULT_QUANTUM;

void schedule() {
    PCB *current = get_current_process();

    if (current && current->state == RUNNING) {
        if (--quantum_remaining > 0)
            return;
        set_process_state(current, READY);
    }

    PCB *next = pick_next_process();
    if (!next)
        next = get_idle_pcb();

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

    do {
        if (curr->pcb->state == READY && curr->pcb->priority > max_priority) {
            max_priority = curr->pcb->priority;
        }
        curr = curr->next ? curr->next : get_active_process_list();
    } while (curr != start);

    curr = start;
    do {
        if (curr->pcb->state == READY && curr->pcb->priority == max_priority) {
            last_rr = curr;
            return curr->pcb;
        }
        curr = curr->next ? curr->next : get_active_process_list();
    } while (curr != start);

    return NULL;
}

void save_context(PCB *pcb) {
    __asm__ volatile("mov %%rsp, %0" : "=g"(pcb->stack_pointer));
}

void load_context(PCB *pcb) {
    __asm__ volatile("mov %0, %%rsp\n"
                     "ret"
                     :
                     : "g"(pcb->stack_pointer));
}
<<<<<<< Updated upstream
=======

void start_scheduler() {
    PCB *next = pick_next_process();
    if (!next)
        next = get_idle_pcb();

    set_current_process(next);
    set_process_state(next, RUNNING);
    load_context(next);
}

>>>>>>> Stashed changes
