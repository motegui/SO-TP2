#include "include/process_manager.h"
#include "include/naiveConsole.h"  // Para ncPrint si necesitás imprimir
#include <stdarg.h>
#include <stdint.h>
#include "scheduler.h"
#include <videodriver.h>
#include <print_utils.h>

extern MemoryManagerADT globalMemoryManager;
static PCBNode *active_processes = NULL;
static PCB *current_process = NULL;
static int next_pid = 1;



PCB *create_process(const char *name, int parent_pid, int priority, bool foreground, void *entry_point, char **args){
    PCB *pcb = allocMemory(globalMemoryManager, sizeof(PCB));
    if (!pcb) return NULL;

    pcb->pid = next_pid++;
    pcb->parent_pid = parent_pid;
    pcb->state = NEW;
    pcb->name = strdup_kernel(name);
    pcb->priority = priority;
    pcb->foreground = foreground;
    pcb->ticks = 0;

    // Asignar stack
    pcb->stack_base = allocMemory(globalMemoryManager, 8192);
    if (!pcb->stack_base) {
        freeMemory(globalMemoryManager, pcb->name);
        freeMemory(globalMemoryManager, pcb);
        return NULL;
    }
    
    uint64_t *stack_top = (uint64_t *)((uint64_t)pcb->stack_base + 8192);
    stack_top = (uint64_t *)((uint64_t)stack_top & ~0xF);

    pcb->stack_pointer = create_stack(stack_top, entry_point, args, &process_wrapper);

    pcb->state = READY;

    char sem_name[16];
    snprintf(sem_name, sizeof(sem_name), "sem_%d", pcb->pid);
    pcb->sem_id = sem_create(0);

    add_active_process(pcb);

    
    return pcb;
    
}
// Función auxiliar para duplicar cadenas sin strdup
char *strdup_kernel(const char *src) {
    size_t len = 0;
    while (src[len]) len++;

    char *dest = allocMemory(globalMemoryManager, len + 1);
    if (!dest) return NULL;

    for (size_t i = 0; i <= len; i++) {
        dest[i] = src[i];
    }

    return dest;
}

void *create_stack(void *stack_top, void *entry_point, char **args, void *wrapper) {
    uint64_t *sp = (uint64_t *)stack_top;

    // Alineamos el stack
    sp = (uint64_t *)((uint64_t)sp & ~0xF);

    *(--sp) = 0x0;                 // SS
    *(--sp) = (uint64_t)stack_top; // RSP
    *(--sp) = 0x202;               // RFLAGS
    *(--sp) = 0x8;                 // CS
    *(--sp) = (uint64_t)wrapper;   // RIP (process_wrapper)

    *(--sp) = 0x0;                 // RAX
    *(--sp) = 0x1;                 // RBX
    *(--sp) = 0x2;                 // RCX
    *(--sp) = 0x3;                 // RDX
    *(--sp) = 0x4;                 // RBP
    *(--sp) = (uint64_t)entry_point; // RDI
    *(--sp) = (uint64_t)args;        // RSI
    *(--sp) = 0x8;                 // R8
    *(--sp) = 0x9;                 // R9
    *(--sp) = 0x10;                // R10
    *(--sp) = 0x11;                // R11
    *(--sp) = 0x12;                // R12
    *(--sp) = 0x13;                // R13
    *(--sp) = 0x14;                // R14
    *(--sp) = 0x15;                // R15

    return (void *)sp;  // stack pointer del proceso
}



void process_wrapper(int (*entry_point)(int, char **), char **args) {
    ((int (*)(int, char **))entry_point)(1, args);
    exit_process();
}

void set_process_state(PCB *pcb, ProcessState new_state) {
    if (pcb) {
        pcb->state = new_state;
    }
}

void set_process_priority(PCB *pcb, int new_priority) {
    if (pcb) {
        pcb->priority = new_priority;
    }
}

void set_process_name(PCB *pcb, const char *new_name) {
    if (pcb && new_name) {
        freeMemory(globalMemoryManager, pcb->name);
        pcb->name = strdup_kernel(new_name);
    }
}

void add_active_process(PCB *pcb) {
    PCBNode *node = allocMemory(globalMemoryManager, sizeof(PCBNode));
    node->pcb = pcb;
    node->next = active_processes;
    active_processes = node;
}

void remove_active_process(int pid) {
    PCBNode **curr = &active_processes;
    while (*curr) {
        if ((*curr)->pcb->pid == pid) {
            PCBNode *to_delete = *curr;
            *curr = (*curr)->next;
            destroy_process(to_delete->pcb);
            freeMemory(globalMemoryManager, to_delete);
            return;
        }
        curr = &((*curr)->next);
    }
}
void freeProcessesInfo(processInfo **info) {
    for (int i = 0; info[i] != NULL; i++) {
        freeMemory(globalMemoryManager, info[i]->name);
        freeMemory(globalMemoryManager, info[i]);
    }
    freeMemory(globalMemoryManager, info);
}


void print_active_processes() {
    PCBNode *curr = active_processes;
    const char *state_str[] = {"NEW", "READY", "RUNNING", "BLOCKED", "ZOMBIE", "TERMINATED"};

    ncPrint("PID\tNombre\t\tEstado\t\tPrioridad\tFG/BG\n");
    while (curr) {
        PCB *pcb = curr->pcb;

        ncPrintDec(pcb->pid);
        ncPrint("\t");
        ncPrint(pcb->name);
        ncPrint("\t\t");
        ncPrint(state_str[pcb->state]);
        ncPrint("\t\t");
        ncPrintDec(pcb->priority);
        ncPrint("\t\t");
        ncPrint(pcb->foreground ? "FG" : "BG");
        ncPrint("\n");

        curr = curr->next;
    }
}

void destroy_process(PCB *pcb) {
    if (!pcb) return;
    pcb->state = TERMINATED;
    freeMemory(globalMemoryManager, pcb->name);
    freeMemory(globalMemoryManager, pcb->stack_base);
    freeMemory(globalMemoryManager, pcb);
}

PCB *get_process_by_pid(int pid) {
    PCBNode *curr = active_processes;
    while (curr) {
        if (curr->pcb->pid == pid) {
            return curr->pcb;
        }
        curr = curr->next;
    }
    return NULL;
}

PCB *get_current_process() {
    return current_process;
}

void set_current_process(PCB *pcb) {
    current_process = pcb;
}

PCBNode *get_active_process_list() {
    return active_processes;
}

void idle_process() {
    while (1) {
        __asm__("hlt");
    }
}

PCB *get_idle_pcb() {
    static PCB idle;
    idle.pid = -1;
    idle.name = "idle";
    idle.priority = 0;
    idle.state = READY;
    return &idle;
}

PCBNode *find_node_by_pid(int pid) {
    PCBNode *curr = active_processes;
    while (curr) {
        if (curr->pcb && curr->pcb->pid == pid) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void nice_process(int pid, int new_priority){
    PCB * target = get_process_by_pid((int) pid);
    if(!target ||(int) new_priority < 0){
        return;
    }
    set_process_priority(target, (int) new_priority);

}

void block_process(int pid){
    PCB * target = get_process_by_pid((int) pid);
    if(!target || target->state == TERMINATED){
        return;
    }
    set_process_state(target, BLOCKED);
}

void unblock_process(int pid){
    PCB * target = get_process_by_pid(pid);
    if(!target || target->state == TERMINATED || target->state != BLOCKED){
        return;
    }
    set_process_state(target, READY);
}


void yield() {

    PCB *current = get_current_process();
    set_process_state(current, READY);
    __asm__ volatile("int $0x20");
}

void wait_for_children(){
    PCB * current_proc = get_current_process();

    while(1){
        int any_alive = 0;
        PCBNode * curr = get_active_process_list();

        while(curr){
            PCB * child = curr->pcb;
            if(child->parent_pid == current_proc->pid && child->state != TERMINATED){
                any_alive = 1;
                break;
            }
            curr = curr->next;
        }

        if(!any_alive){
            return;
        }
        yield(); //cedo el paso a otro proceso
    }
}

void exit_process() {
    PCB *current = get_current_process();
    if (!current) return;

    if (strcmp(current->name, "sh") == 0) {
        return;
    }

    current->state = ZOMBIE;
    sem_post(current->sem_id);

    __asm__ volatile("int $0x20");
}

void kill_process(int pid) {
    PCB *target = get_process_by_pid(pid);
    if (!target) {
        ncPrint("Error: Proceso no encontrado.\n");
        return;
    }

    if (target->state == ZOMBIE || get_process_by_pid(target->parent_pid) == NULL) {

        target->state = TERMINATED;
        remove_active_process(target->pid);
        destroy_process(target);
    } else {
        target->state = ZOMBIE;
        sem_post(target->sem_id);

    }

}


void get_pid() {
    PCB *current = get_current_process();
    if (current) {
        ncPrintDec(current->pid);
        ncPrint("\n");
    } else {
        ncPrint("No hay proceso actual.\n");
    }
}

void list_processes(char *buffer, uint64_t length) {
    static const char *state_str[] = {"NEW", "READY", "RUNNING", "BLOCKED", "TERMINATED", "ZOMBIE"};
    PCBNode *curr = active_processes;
    int offset = 0;

    while (curr && offset < (int)length - 1) {
        PCB *pcb = curr->pcb;
        const char *state = (pcb->state >= 0 && pcb->state <= 6) ? state_str[pcb->state] : "UNKNOWN";
        int written = snprintf(
            buffer + offset, length - offset,
            "PID: %d, Name: %s, State: %s, Priority: %d\n",
            pcb->pid, pcb->name, state, pcb->priority
        );
        if (written < 0 || written >= (int)(length - offset)) {
            buffer[length - 1] = '\0';
            return;
        }
        offset += written;
        curr = curr->next;
    }

    if (offset < (int)length)
        buffer[offset] = '\0';
    else
        buffer[length - 1] = '\0';
}

int wait_pid(int pid) {
    PCB *current_proc = get_current_process();
    if (!current_proc) return -1;

    PCB *target = get_process_by_pid(pid);
    if (!target || target->parent_pid != current_proc->pid) {
        return -1;
    }

    sem_wait(target->sem_id);          // Bloquea y espera al hijo
    kill_process(target->pid);        // Hace cleanup
    return pid;
}

