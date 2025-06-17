#include <usyscalls.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <programs.h>

#define MAX_PHILOS 10
#define MIN_PHILOS 2

#define THINKING 0
#define HUNGRY   1
#define EATING   2

static int philosopher_pids[MAX_PHILOS];
static char fork_names[MAX_PHILOS][8] = {
    "fork0","fork1","fork2","fork3","fork4",
    "fork5","fork6","fork7","fork8","fork9"
};

static int running_philos = 5;
static volatile int states[MAX_PHILOS];
static uint64_t state_sem = (uint64_t)"state_sem";

void sleep_cycles(int ms) {
    for (volatile int i = 0; i < ms * 10000; i++);
}

int str_to_int(char *str) {
    int res = 0;
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }
    return res;
}

void int_to_str(int num, char *str) {
    int i = 0;
    if (num == 0) {
        str[i++] = '0';
    } else {
        char tmp[10];
        while (num > 0) {
            tmp[i++] = '0' + (num % 10);
            num /= 10;
        }
        for (int j = 0; j < i; j++) str[j] = tmp[i - j - 1];
    }
    str[i] = 0;
}

void print_states() {
    sys_sem_wait(state_sem);
    for (int i = 0; i < running_philos; i++) {
        char c = (states[i] == EATING) ? 'E' : '.';
        sys_write(1, &c, 1);
        sys_write(1, " ", 1);
    }
    sys_write(1, "\n", 1);
    sys_sem_post(state_sem);
}

int printer(int argc, char **argv) {
    while (1) {
        print_states();
        sleep_cycles(150);
    }
    return 0;
}

// ------------ FILÓSOFO ------------------

int philosopher_main(int argc, char **argv) {
    if (argc < 3) return -1;
    int id = str_to_int(argv[1]);
    int total = str_to_int(argv[2]);

    char *left_fork = fork_names[id];
    char *right_fork = fork_names[(id + 1) % total];

    while (1) {
        sys_sem_wait(state_sem);
        states[id] = THINKING;
        sys_sem_post(state_sem);
        sleep_cycles(50);

        sys_sem_wait(state_sem);
        states[id] = HUNGRY;
        sys_sem_post(state_sem);

        if (id % 2 == 0) {
            sys_sem_wait((uint64_t)left_fork);
            sys_sem_wait((uint64_t)right_fork);
        } else {
            sys_sem_wait((uint64_t)right_fork);
            sys_sem_wait((uint64_t)left_fork);
        }

        sys_sem_wait(state_sem);
        states[id] = EATING;
        sys_sem_post(state_sem);
        sleep_cycles(50);

        sys_sem_post((uint64_t)left_fork);
        sys_sem_post((uint64_t)right_fork);
    }

    return 0;
}

// ------------ CONTROLADOR PRINCIPAL ------------------

void start_philosopher(int id, int total) {
    char id_str[5], total_str[5];
    int_to_str(id, id_str);
    int_to_str(total, total_str);
    char *args[] = { "philosopher", id_str, total_str, NULL };
    philosopher_pids[id] = sys_create_process("philosopher", 1, 1, (void *)philosopher_main, args);
}

void stop_philosopher(int id) {
    if (philosopher_pids[id] > 0) {
        sys_kill_process(philosopher_pids[id]);
        philosopher_pids[id] = 0;
    }
}

int phylo(int argc, char **argv) {
    sys_sem_create(state_sem, 1);

    for (int i = 0; i < MAX_PHILOS; i++) {
        sys_sem_create((uint64_t)fork_names[i], 1);
        states[i] = THINKING;
        philosopher_pids[i] = 0;
    }

    for (int i = 0; i < running_philos; i++) {
        start_philosopher(i, running_philos);
    }

    sys_create_process("printer", 1, 1, (void *)printer, NULL);

    sys_write(1, "Presione 'a' para agregar, 'r' para quitar filosofos.\n", 53);

    while (1) {
        char c = 0;
        while (sys_read(0, &c, 1) == 1) {
            if (c == 'a' && running_philos < MAX_PHILOS) {
                start_philosopher(running_philos, running_philos + 1);
                running_philos++;
            } else if (c == 'r' && running_philos > MIN_PHILOS) {
                running_philos--;
                stop_philosopher(running_philos);
            }
        }
        sleep_cycles(30);
    }

    return 0;
}