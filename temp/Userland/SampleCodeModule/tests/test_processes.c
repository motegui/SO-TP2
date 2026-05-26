#include "usyscalls.h"
#include "test_util.h"
#include <userio.h>

#define MAX_TEST_PROCS 32

enum ProcessState { P_RUNNING, P_BLOCKED, P_KILLED };

typedef struct P_rq {
  int32_t pid;
  enum ProcessState state;
} p_rq;

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
    int max_processes;
    static char wait_arg[] = "100000";
    char *argvAux[] = {wait_arg, NULL};

    if (argc != 1) {
        return -1;
    }

    if ((max_processes = satoi(argv[0])) <= 0) {
        return -1;
    }

    if (max_processes > MAX_TEST_PROCS) {
        max_processes = MAX_TEST_PROCS;
    }

    p_rq p_rqs[MAX_TEST_PROCS];

    sys_write(1, "\n[testpro] start\n", 16);

    while (1) {
        for (rq = 0; rq < max_processes; rq++) {
            p_rqs[rq].pid = (int32_t) sys_create_process("test_process", 0, 0,
                (void *) endless_loop_print, argvAux);
            if (p_rqs[rq].pid <= 0) {
                sys_write(1, "test_processes: ERROR creating process\n", 39);
                return -1;
            }
            p_rqs[rq].state = P_RUNNING;
            alive++;
        }

        while (alive > 0) {
            for (rq = 0; rq < max_processes; rq++) {
                action = GetUniform(100) % 2;

                switch (action) {
                    case 0:
                        if (p_rqs[rq].state == P_RUNNING || p_rqs[rq].state == P_BLOCKED) {
                            sys_kill_process(p_rqs[rq].pid);
                            p_rqs[rq].state = P_KILLED;
                            alive--;
                        }
                        break;

                    case 1:
                        if (p_rqs[rq].state == P_RUNNING) {
                            my_block(p_rqs[rq].pid);
                            p_rqs[rq].state = P_BLOCKED;
                        }
                        break;
                }
            }

            for (rq = 0; rq < max_processes; rq++) {
                if (p_rqs[rq].state == P_BLOCKED && GetUniform(100) % 2) {
                    my_unblock(p_rqs[rq].pid);
                    p_rqs[rq].state = P_RUNNING;
                }
            }
        }
    }
}
