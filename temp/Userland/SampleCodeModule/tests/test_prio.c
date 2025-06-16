#include <stdint.h>
#include "usyscalls.h"
#include "test_util.h"

#define MINOR_WAIT 1000000
#define WAIT 10000000

#define TOTAL_PROCESSES 3
#define LOWEST 0
#define MEDIUM 1
#define HIGHEST 2

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
    int64_t pids[TOTAL_PROCESSES];
    char *argv[] = {0};
    uint64_t i;

    for (i = 0; i < TOTAL_PROCESSES; i++)
        pids[i] = sys_create_process("endless_loop_print", 0, 0, (void *)endless_loop_print, argv);

    bussy_wait(WAIT);
    sys_write(1, "\nCHANGING PRIORITIES...\n", 24);

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sys_nice_process(pids[i], prio[i]);

    bussy_wait(WAIT);
    sys_write(1, "\nBLOCKING...\n", 13);

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sys_block_process(pids[i]);

    sys_write(1, "CHANGING PRIORITIES WHILE BLOCKED...\n", 37);

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sys_nice_process(pids[i], MEDIUM);

    sys_write(1, "UNBLOCKING...\n", 14);

    for (i = 0; i < TOTAL_PROCESSES; i++){
        sys_unblock_process(pids[i]);
}
    bussy_wait(WAIT);
    sys_write(1, "\nKILLING...\n", 12);

    for (i = 0; i < TOTAL_PROCESSES; i++)
        sys_kill_process(pids[i]);
}