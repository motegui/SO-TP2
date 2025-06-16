#include "scheduler.h"
#include "process_manager.h"
#include <videodriver.h>

#define DEFAULT_QUANTUM 5


static int quantum_remaining = DEFAULT_QUANTUM;

uint64_t schedule(uint64_t current_rsp) {
    PCB *current = get_current_process();
    if (current && current->state == RUNNING) {
        save_context(current, current_rsp);
        current->state = READY;
    }

    PCB *next = pick_next_process();
    if (!next){
       // printStringNColor("[SCHED] if", 24, (Color){255, 255, 0});
        next = get_idle_pcb();
    }
    set_current_process(next);
    next->state = RUNNING;

    //printStringNColor("[SCHED] Eligiendo proceso: ", 24, (Color){255, 255, 0});
  // printString(next->name);
    //printStringNColor("\n", 24, (Color){255, 255, 0} );

    return next->stack_pointer;
}


PCB *pick_next_process() {
    //printStringNColor("[KERNEL] pick np\n", 24, (Color){255, 150, 0});
    PCBNode *curr = get_active_process_list();
    if (!curr) return NULL;

    PCB *best = NULL;
    int max_priority = -1;
    PCBNode *start = curr;
    do {
        if (curr->pcb->state == READY && curr->pcb->priority > max_priority) {
            best = curr->pcb;
            max_priority = curr->pcb->priority;

        }
        curr = curr->next ? curr->next : get_active_process_list();

    } while (curr != start);

   // printString(best->name);


    return best;
}

void save_context(PCB *pcb, uint64_t rsp) {
    pcb->stack_pointer = (void *)rsp;
}


void load_context(PCB *pcb) {
    printStringNColor("[KERNEL] load\n", 24, (Color){255, 150, 0});

    __asm__ volatile (
        "mov %[stack], %%rsp\n"
        "mov (%%rsp), %%rdi\n"       // RDI ← entry_point
        "mov 8(%%rsp), %%rsi\n"      // RSI ← args
        "jmp *16(%%rsp)\n"           // saltar a wrapper
        :
        : [stack] "r"(pcb->stack_pointer)
        : "rdi", "rsi"
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

