#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#define PROCESS_MANAGER_H

#include <stdbool.h>
#include "mm_manager.h"
#include <stdint.h>
#include <unistd.h>
#include "lib.h"

typedef enum {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} ProcessState;


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

typedef struct PCBNode {
    PCB *pcb;
    struct PCBNode *next;
} PCBNode;

PCB *create_process(const char *name, int parent_pid, int priority, bool foreground, void *entry_point, char **args);

void process_wrapper(int (*entry_point)(int, char **), char **args);

void *create_stack(void *stack_top, void *entry_point, char **args, void *wrapper);

void set_process_state(PCB *pcb, ProcessState new_state);

void set_process_priority(PCB *pcb, int new_priority);

void set_process_name(PCB *pcb, const char *new_name);

void add_active_process(PCB *pcb);

void remove_active_process(int pid);

void print_active_processes();

void destroy_process(PCB *pcb);

PCB *get_process_by_pid(int pid);

PCB *get_current_process();

void set_current_process(PCB *pcb);

void idle_process();

void test_process_manager();

PCBNode *get_active_process_list();

PCB *create_idle_process();

PCB *get_idle_pcb();

void nice_process(int pid, int new_priority);

void block_process(int pid);

void unblock_process(int pid);

void yield();

void wait_for_children();

void exit_process();

void kill_process(int pid);

void get_pid();

void list_processes(char *buffer, uint64_t length);
int waitpid(int pid);

#endif 
