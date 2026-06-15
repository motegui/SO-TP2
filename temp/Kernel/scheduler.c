#include "scheduler.h"
#include "process_manager.h"
#include "sync.h"
#include <videodriver.h>
#include <time.h>

#define DEFAULT_QUANTUM 5

static int quantum_remaining = DEFAULT_QUANTUM;

void save_context(PCB *pcb, uint64_t rsp) {
    pcb->stack_pointer = (void *)rsp;
}

uint64_t schedule(uint64_t current_rsp) {
    timer_handler();
    PCB *current = get_current_process();
    if (current) {
        save_context(current, current_rsp);

        if (is_blocked_by_semaphore(current->pid)) {
            current->state = BLOCKED;
        }

        // Si el proceso actual todavia puede correr (no se bloqueo ni termino),
        // le queda quantum y lo dejamos seguir. El quantum es proporcional a la
        // prioridad, asi los de mayor prioridad reciben mas CPU, pero igual
        // rotamos round-robin para no matar de hambre a los de menor prioridad.
        if (current->state == RUNNING || current->state == READY) {
            if (--quantum_remaining > 0) {
                current->state = RUNNING;
                return (uint64_t) current->stack_pointer;
            }
            current->state = READY;
        }
    }

    PCB *next = pick_next_process();
    if (!next){
        next = get_idle_pcb();
    }
    set_current_process(next);
    next->state = RUNNING;
    quantum_remaining = next->priority + 1;

    return (uint64_t)next->stack_pointer;
}

PCB *pick_next_process() {
    PCBNode *head = get_active_process_list();
    if (!head) return NULL;

    PCB *best = NULL;

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

    // Round-robin: tomamos el primer proceso READY a partir del siguiente al
    // actual. El peso por prioridad lo da el quantum (ver schedule), no la
    // seleccion, de modo que ningun proceso queda sin correr.
    do {
        if (curr->pcb->state == READY) {
            best = curr->pcb;
            break;
        }
        curr = curr->next ? curr->next : head;

    } while (curr != start);

    return best;
}
int get_foreground_pid() {
    return get_foreground_process_pid();
}
