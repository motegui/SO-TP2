#include "usyscalls.h"
#include "test_util.h"

enum State { RUNNING, BLOCKED, KILLED };

typedef struct P_rq {
  int32_t pid;
  enum State state;
} p_rq;

// Funciones auxiliares para bloquear y desbloquear procesos usando syscalls
int my_block(int32_t pid) {
    sys_block_process(pid);
    return 0; 
}

int my_unblock(int32_t pid) {
    sys_unblock_process(pid);
    return 0; 
}

int64_t test_processes(uint64_t argc, char *argv[]) {
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;
    uint64_t max_processes;
    char *argvAux[] = {0};

    if (argc != 1)
        return -1;

    if ((max_processes = satoi(argv[0])) <= 0)
        return -1;

    p_rq p_rqs[max_processes];

    while (1) {
        // Create max_processes processes
        for (rq = 0; rq < max_processes; rq++) {
            p_rqs[rq].pid = sys_create_process("test_processes", 0, 0, (void *)endless_loop_print, argvAux);
            if (p_rqs[rq].pid == -1) {
                sys_write(1, "test_processes: ERROR creating process\n", 39);
                return -1;
            } else {
                p_rqs[rq].state = RUNNING;
                alive++;
            }
        }

        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0) {
            for (rq = 0; rq < max_processes; rq++) {
                action = GetUniform(100) % 2;

                switch (action) {
                    case 0:
                        if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
                            sys_kill_process(p_rqs[rq].pid);
                            p_rqs[rq].state = KILLED;
                            alive--;
                        }
                        break;

                    case 1:
                        if (p_rqs[rq].state == RUNNING) {
                            my_block(p_rqs[rq].pid);
                            p_rqs[rq].state = BLOCKED;
                        }
                        break;
                }
            }

            // Randomly unblocks processes
            for (rq = 0; rq < max_processes; rq++)
                if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
                    my_unblock(p_rqs[rq].pid);
                    p_rqs[rq].state = RUNNING;
                }
        }
    }
}