#include <usyscalls.h>
#include <stdint.h>
#include <stdbool.h>
#include <programs.h>

#define MAX_PHILOS 10
#define MIN_PHILOS 2

#define THINKING 0
#define HUNGRY   1
#define EATING   2

static int state[MAX_PHILOS];
static int running_philos = 5;
static char fork_names[MAX_PHILOS][8] = {"fork0","fork1","fork2","fork3","fork4","fork5","fork6","fork7","fork8","fork9"};
static int philosopher_pids[MAX_PHILOS];
static int64_t forks[MAX_PHILOS];

static int next_id = 0;
static uint64_t id_sem;

void print_table() {
    char line[MAX_PHILOS+2];
    for (int i = 0; i < running_philos; i++) {
        line[i] = (state[i] == EATING) ? 'E' : '.';
    }
    line[running_philos] = '\n';
    line[running_philos+1] = 0;
    sys_write(1, line, running_philos+1);
}

void sleep_cycles(int ms) {
    for (volatile int i = 0; i < ms * 10000; i++);
}

void philosopher() {
    sys_sem_wait(id_sem);
    int id = next_id++;
    sys_sem_post(id_sem);

    while (1) {
        int n = running_philos; // Captura el valor actual
        state[id] = THINKING;
        sleep_cycles(50);

        state[id] = HUNGRY;

        int left = id;
        int right = (id + 1) % n;
        if (left < right) {
            sys_sem_wait((uint64_t)fork_names[left]);
            sys_sem_wait((uint64_t)fork_names[right]);
        } else {
            sys_sem_wait((uint64_t)fork_names[right]);
            sys_sem_wait((uint64_t)fork_names[left]);
        }

        state[id] = EATING;
        sleep_cycles(50);

        sys_sem_post((uint64_t)fork_names[left]);
        sys_sem_post((uint64_t)fork_names[right]);
    }
}

void start_philosopher(int id) {
    philosopher_pids[id] = sys_create_process((char *)"philo_child", 1, 1, 1);
}

void stop_philosopher(int id) {
    if (philosopher_pids[id] > 0) {
        sys_kill_process(philosopher_pids[id]);
        philosopher_pids[id] = 0;
        state[id] = THINKING;
    }
}

int phylo(int argc, char **argv) {
    // Inicializar semáforos para tenedores
    for (int i = 0; i < MAX_PHILOS; i++) {
        sys_sem_create((uint64_t)fork_names[i], 1);
        state[i] = THINKING;
        philosopher_pids[i] = 0;
    }

    id_sem = sys_sem_create((uint64_t)"id_sem", 1);
    next_id = 0;

    //inicio filosofos
    for (int i = 0; i < running_philos; i++) {
        start_philosopher(i);
    }

    sys_write(1, "Presione 'a' para agregar, 'r' para quitar filosofos.\n", 53);

    while (1) {
        print_table();

        char c = 0;
        while (sys_read(0, &c, 1) == 1) {
            if (c == 'a' && running_philos < MAX_PHILOS) {
                start_philosopher(running_philos);
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