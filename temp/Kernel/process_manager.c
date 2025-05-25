/*
1. Estructura PCB ( guardar toda la info de un proceso)
2.Funciones para crear y destruir procesos
3. Funciones para modificar procesos
4.Lista de procesos activos
5. Iniciar el proceso que tiene que correr cuando no hay nada mas que ejecutar (idle)
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "include/process_manager.h"

typedef struct PCBNode {
    PCB *pcb;
    struct PCBNode *next;
} PCBNode;

static PCBNode *active_processes = NULL;
static PCB *current_process = NULL;

static int next_pid = 1; // PID incremental

// Crear un nuevo proceso
PCB *create_process(const char *name, int parent_pid, int priority, bool foreground) {
    PCB *pcb = malloc(sizeof(PCB));
    if (!pcb) return NULL;

    pcb->pid = next_pid++;
    pcb->parent_pid = parent_pid;
    pcb->state = NEW;
    pcb->name = strdup(name);
    pcb->priority = priority;
    pcb->foreground = foreground;
    pcb->ticks = 0;

    // Asignar stack (ejemplo: 8KB)
    pcb->stack_base = malloc(8192);
    if (!pcb->stack_base) {
        free(pcb->name);
        free(pcb);
        return NULL;
    }

    pcb->stack_pointer = (void *)((char *)pcb->stack_base + 8192); // stack crece hacia abajo

    add_active_process(pcb);
    return pcb;
}

// Cambiar estado del proceso
void set_process_state(PCB *pcb, ProcessState new_state) {
    if (pcb) {
        pcb->state = new_state;
    }
}

// Cambiar prioridad
void set_process_priority(PCB *pcb, int new_priority) {
    if (pcb) {
        pcb->priority = new_priority;
    }
}

// Cambiar nombre
void set_process_name(PCB *pcb, const char *new_name) {
    if (pcb && new_name) {
        free(pcb->name);
        pcb->name = strdup(new_name);
    }
}

// Agregar a la lista de procesos activos
void add_active_process(PCB *pcb) {
    PCBNode *node = malloc(sizeof(PCBNode));
    node->pcb = pcb;
    node->next = active_processes;
    active_processes = node;
}

// Eliminar un proceso de la lista por PID
void remove_active_process(int pid) {
    PCBNode **curr = &active_processes;
    while (*curr) {
        if ((*curr)->pcb->pid == pid) {
            PCBNode *to_delete = *curr;
            *curr = (*curr)->next;
            destroy_process(to_delete->pcb);
            free(to_delete);
            return;
        }
        curr = &((*curr)->next);
    }
}

// Listar procesos activos
void print_active_processes() {
    PCBNode *curr = active_processes;
    const char *state_str[] = {"NEW", "READY", "RUNNING", "BLOCKED", "TERMINATED"};

    printf("PID\tNombre\t\tEstado\t\tPrioridad\tFG/BG\n");
    while (curr) {
        PCB *pcb = curr->pcb;
        printf("%d\t%s\t\t%s\t\t%d\t\t%s\n",
               pcb->pid,
               pcb->name,
               state_str[pcb->state],
               pcb->priority,
               pcb->foreground ? "FG" : "BG");
        curr = curr->next;
    }
}

// Liberar memoria del proceso
void destroy_process(PCB *pcb) {
    if (!pcb) return;
    pcb->state = TERMINATED;
    free(pcb->name);
    free(pcb->stack_base);
    free(pcb);
}

// Buscar proceso por PID
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

// Obtener el proceso actualmente en ejecución
PCB *get_current_process() {
    return current_process;
}

// Establecer el proceso actual (usado por el scheduler)
void set_current_process(PCB *pcb) {
    current_process = pcb;
}

// Proceso idle (cuando no hay nada para ejecutar)
void idle_process() {
    while (1) {
        __asm__("hlt");
    }
}

// Prueba básica (opcional)
void test_process_manager() {
    PCB *p1 = create_process("proc1", 0, 1, true);
    PCB *p2 = create_process("proc2", 0, 2, false);
    print_active_processes();
    remove_active_process(p1->pid);
    print_active_processes();
}