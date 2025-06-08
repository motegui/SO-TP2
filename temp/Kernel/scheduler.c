#include "scheduler.h"
#include "process_manager.h"
#include <videodriver.h>

#define DEFAULT_QUANTUM 5


static int quantum_remaining = DEFAULT_QUANTUM;

void schedule() {
    PCB *current = get_current_process();

    // Si aún le queda quantum, seguimos
    if (current && current->state == RUNNING) {
        if (--quantum_remaining > 0)
            return;
        set_process_state(current, READY);
    }

    // Contar cuántos procesos READY hay además del shell
    int ready_count = 0;
    PCB *shell = NULL;
    PCBNode *curr = get_active_process_list();
    if (curr) {
        PCBNode *start = curr;
        do {
            if (curr->pcb->state == READY) {
                if (strcmp(curr->pcb->name, "sh") == 0)
                    shell = curr->pcb;
                else
                    ready_count++;
            }
            curr = curr->next ? curr->next : get_active_process_list();
        } while (curr != start);
    }

    // Si no hay READY que no sean la shell → seguir con shell
    if (ready_count == 0 && current == shell) {
        set_process_state(current, RUNNING);
        quantum_remaining = DEFAULT_QUANTUM;
        return;
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
    printStringNColor("[KERNEL] pick np\n", 24, (Color){255, 150, 0});

    static PCBNode *last_rr = NULL;
    PCBNode *start = last_rr ? last_rr->next : get_active_process_list();
    if (!start) return NULL;

    PCB *shell = NULL;
    PCB *next = NULL;

    int max_priority = -1;
    PCBNode *curr = start;
    

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
    printStringNColor("[KERNEL] load\n", 24, (Color){255, 150, 0});

    __asm__ volatile(
        "mov %0, %%rsp\n"    // Cargamos el stack pointer del proceso
        "pop %%rdi\n"        // Primer argumento → RDI (entry_point)
        "pop %%rsi\n"        // Segundo argumento → RSI (args)
        "ret\n"              // Salta a wrapper(entry_point, args)
        :
        : "g"(pcb->stack_pointer)
    );
}


void start_scheduler() {
    printStringNColor("[KERNEL] shceduler\n", 24, (Color){255, 150, 0});

    PCB *next = pick_next_process();
    if (!next)
        next = get_idle_pcb();


    set_current_process(next);
    set_process_state(next, RUNNING);
    load_context(next);
}

