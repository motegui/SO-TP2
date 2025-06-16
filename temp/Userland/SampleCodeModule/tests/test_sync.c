#include <stdint.h>
#include "usyscalls.h"
#include "test_util.h"

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
    uint64_t aux = *p;
    sys_yield(); // This makes the race condition highly probable
    aux += inc;
    *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
    uint64_t n;
    int8_t inc;
    int8_t use_sem;

    if (argc != 3)
        return -1;

    if ((n = satoi(argv[0])) <= 0)
        return -1;
    if ((inc = satoi(argv[1])) == 0)
        return -1;
    if ((use_sem = satoi(argv[2])) < 0)
        return -1;

    if (use_sem)
        if (!sys_sem_create(SEM_ID, 1)) {
            sys_write(1, "test_sync: ERROR opening semaphore\n", 35);
            return -1;
        }

    for (uint64_t i = 0; i < n; i++) {
        if (use_sem)
            sys_sem_wait(SEM_ID);
        slowInc(&global, inc);
        if (use_sem)
            sys_sem_post(SEM_ID);
    }

    if (use_sem)
        sys_sem_close(SEM_ID);

    return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
    uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

    if (argc != 2)
        return -1;

    char *argvDec[] = {argv[0], "-1", argv[1], NULL};
    char *argvInc[] = {argv[0], "1", argv[1], NULL};

    global = 0;

    for (uint64_t i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
      pids[i] = sys_create_process("my_process_inc", 3, 1, (void *)my_process_inc, argvDec);
      pids[i + TOTAL_PAIR_PROCESSES] = sys_create_process("my_process_inc", 3, 1, (void *)my_process_inc, argvInc);
    }

    for (uint64_t i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        sys_wait_pid(pids[i]);
        sys_wait_pid(pids[i + TOTAL_PAIR_PROCESSES]);
    }

    // Imprimir el valor final de global
    char buffer[64];
    int len = 0, tmp = global;
    if (tmp == 0) {
        buffer[len++] = '0';
    } else {
        int is_negative = 0;
        if (tmp < 0) {
            is_negative = 1;
            tmp = -tmp;
        }
        char numbuf[32];
        int numlen = 0;
        while (tmp > 0 && numlen < 31) {
            numbuf[numlen++] = '0' + (tmp % 10);
            tmp /= 10;
        }
        if (is_negative)
            buffer[len++] = '-';
        for (int j = numlen - 1; j >= 0; j--)
            buffer[len++] = numbuf[j];
    }
    buffer[len++] = '\n';
    sys_write(1, "Final value: ", 13);
    sys_write(1, buffer, len);

    return 0;
}