#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#define PROCESS_MANAGER_H

#include <stdbool.h>

// Estados posibles de un proceso
typedef enum {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} ProcessState;

// Estructura del PCB (Process Control Block)
typedef struct PCB {
    int pid;
    int parent_pid;
    ProcessState state;
    char *name;
    int priority;
    bool foreground;
    int ticks;
    void *stack_base;
    void *stack_pointer;
} PCB;

// Crear un nuevo proceso
PCB *create_process(const char *name, int parent_pid, int priority, bool foreground);

// Cambiar estado de un proceso
void set_process_state(PCB *pcb, ProcessState new_state);

// Cambiar prioridad de un proceso
void set_process_priority(PCB *pcb, int new_priority);

// Cambiar nombre de un proceso
void set_process_name(PCB *pcb, const char *new_name);

// Agregar proceso a la lista de activos
void add_active_process(PCB *pcb);

// Eliminar proceso por PID
void remove_active_process(int pid);

// Imprimir todos los procesos activos
void print_active_processes();

// Liberar la memoria de un proceso
void destroy_process(PCB *pcb);

// Buscar un proceso por su PID
PCB *get_process_by_pid(int pid);

// Obtener el proceso actualmente en ejecución
PCB *get_current_process(void);

// Establecer el proceso actualmente en ejecución
void set_current_process(PCB *pcb);

// Proceso idle (corre cuando no hay otro listo)
void idle_process(void);

// Test interno (opcional)
void test_process_manager(void);

#endif // PROCESS_MANAGER_H
