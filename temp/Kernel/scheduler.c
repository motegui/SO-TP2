#include "scheduler.h"
#include "process_manager.h"
#include "sync.h"
#include <videodriver.h>

#define DEFAULT_QUANTUM 5

static int quantum_remaining = DEFAULT_QUANTUM;

void save_context(PCB *pcb, uint64_t rsp) {
    pcb->stack_pointer = (void *)rsp;
}

uint64_t schedule(uint64_t current_rsp) {
    PCB *current = get_current_process();
    if (current) {
        save_context(current, current_rsp);
        if (is_blocked_by_semaphore(current->pid)) {
            current->state = BLOCKED;
        } else if (current->state == RUNNING) {
            current->state = READY;
        }
    }

    PCB *next = pick_next_process();
    if (!next){
        next = get_idle_pcb();
    }
    set_current_process(next);
    next->state = RUNNING;

    return (uint64_t)next->stack_pointer;
}

PCB *pick_next_process() {
    PCBNode *head = get_active_process_list();
    if (!head) return NULL;

    PCB *best = NULL;
    int max_priority = -1;

    PCB *current = get_current_process();
    PCBNode *current_node = NULL;
    PCBNode *curr = head;

    do {
        if (curr->pcb == current) {
            current_node = curr;
            break;
        }
        curr = curr->next ? curr->next : head;
    } while (curr != head);

    PCBNode *start = current_node && current_node->next ? current_node->next : head;
    curr = start;

    do {
        if (curr->pcb->state == READY && curr->pcb->priority > max_priority) {
            best = curr->pcb;
            max_priority = curr->pcb->priority;

        }
        curr = curr->next ? curr->next : head;

    } while (curr != start);

    return best;
}
int get_foreground_pid() {
    int fg = get_foreground_process_pid();
    if (fg > 0) {
        return fg;
    }
    PCB *current = get_current_process();
    return current ? current->pid : -1;
}
