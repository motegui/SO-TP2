#include "include/process_manager.h"
#include "include/naiveConsole.h"  // Para ncPrint si necesitás imprimir


#include <stdarg.h>
#include <stdint.h>

int int_to_str(int value, char *str) {
    char temp[12];
    int i = 0, j = 0;
    int is_negative = 0;

    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value > 0) {
        temp[i++] = (value % 10) + '0';
        value /= 10;
    }

    if (is_negative) {
        temp[i++] = '-';
    }

    while (i > 0) {
        str[j++] = temp[--i];
    }

    str[j] = '\0';
    return j;
}

int snprintf(char *buffer, uint64_t size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    uint64_t offset = 0;
    for (const char *p = fmt; *p && offset < size - 1; ++p) {
        if (*p == '%') {
            p++; // Avanzamos al especificador
            if (!*p) break;

            if (*p == 'd') {
                int val = va_arg(args, int);
                char temp[12];
                int len = int_to_str(val, temp);
                for (int i = 0; i < len && offset < size - 1; i++)
                    buffer[offset++] = temp[i];
            } else if (*p == 's') {
                const char *str = va_arg(args, const char*);
                while (*str && offset < size - 1)
                    buffer[offset++] = *str++;
            } else if (*p == 'c') {
                char c = (char)va_arg(args, int);
                if (offset < size - 1)
                    buffer[offset++] = c;
            } else {
                // No reconocido, lo copiamos literalmente
                if (offset < size - 1) buffer[offset++] = '%';
                if (offset < size - 1) buffer[offset++] = *p;
            }
        } else {
            buffer[offset++] = *p;
        }
    }

    buffer[offset] = '\0';
    va_end(args);
    return offset;
}



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
    PCB * target = get_process_by_pid((int) pid);
    if(!target || target->state == TERMINATED){
        return;
    }
    set_process_state(target, READY);
}

void yield() {
    PCB *current = get_current_process();
    set_process_state(current, READY);
    schedule();
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
            curr->next;
        }

        if(!any_alive){
            return;
        }
        yield(); //cedo el paso a otro proceso
    }
}

void exit_process(){
    PCB *current = get_current_process();
    if (current) {
        remove_active_process(current->pid);
        schedule();
    }
}

void kill_process(int pid){
    PCB *target = get_process_by_pid(pid);
    if (target) {
        remove_active_process(target->pid);
        destroy_process(target);
    }
    else {
        ncPrint("Error: Proceso no encontrado.\n");
    }
    schedule(); // Reprogramar después de eliminar el proceso
    ncPrint("Proceso con PID ");
    ncPrintDec(pid);
    ncPrint(" eliminado.\n");
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
    PCBNode *curr = active_processes;
    int offset = 0;

    while (curr && offset < length - 1) {
        PCB *pcb = curr->pcb;
        int written = snprintf(buffer + offset, length - offset, "PID: %d, Name: %s, State: %d, Priority: %d\n",
                               pcb->pid, pcb->name, pcb->state, pcb->priority);
        if (written < 0 || written >= length - offset) {
            break; // No hay espacio suficiente
        }
        offset += written;
        curr = curr->next;
    }

    if (offset < length) {
        buffer[offset] = '\0'; // Asegurar que el buffer esté terminado en nulo
    }
}

void test_process_manager() {
    PCB *p1 = create_process("proc1", 0, 1, true);
    PCB *p2 = create_process("proc2", 0, 2, false);
    print_active_processes();
    remove_active_process(p1->pid);
    print_active_processes();
}
