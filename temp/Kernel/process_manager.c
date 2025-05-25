#include "include/process_manager.h"
#include "include/naiveConsole.h"  // Para ncPrint si necesitás imprimir

extern MemoryManagerADT globalMemoryManager;

static PCBNode *active_processes = NULL;
static PCB *current_process = NULL;
static int next_pid = 1;

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

PCB *create_process(const char *name, int parent_pid, int priority, bool foreground) {
    PCB *pcb = allocMemory(globalMemoryManager, sizeof(PCB));
    if (!pcb) return NULL;

    pcb->pid = next_pid++;
    pcb->parent_pid = parent_pid;
    pcb->state = NEW;
    pcb->name = strdup_kernel(name);
    pcb->priority = priority;
    pcb->foreground = foreground;
    pcb->ticks = 0;

    // Asignar stack (8KB)
    pcb->stack_base = allocMemory(globalMemoryManager, 8192);
    if (!pcb->stack_base) {
        freeMemory(globalMemoryManager, pcb->name);
        freeMemory(globalMemoryManager, pcb);
        return NULL;
    }

    pcb->stack_pointer = (void *)((char *)pcb->stack_base + 8192); // stack crece hacia abajo

    add_active_process(pcb);
    return pcb;
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

void print_active_processes() {
    PCBNode *curr = active_processes;
    const char *state_str[] = {"NEW", "READY", "RUNNING", "BLOCKED", "TERMINATED"};

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

void test_process_manager() {
    PCB *p1 = create_process("proc1", 0, 1, true);
    PCB *p2 = create_process("proc2", 0, 2, false);
    print_active_processes();
    remove_active_process(p1->pid);
    print_active_processes();
}
