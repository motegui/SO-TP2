#include <usyscalls.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <programs.h>
#include <colors.h>

#define MAX_PROCESSES 20
#define MVAR_EMPTY "mvar_empty"
#define MVAR_FULL "mvar_full"
#define MVAR_MUTEX "mvar_mtx"

static int writer_pids[MAX_PROCESSES];
static int reader_pids[MAX_PROCESSES];
static char writer_id_args[MAX_PROCESSES][5];
static char reader_id_args[MAX_PROCESSES][5];
static char *writer_args[MAX_PROCESSES][3];
static char *reader_args[MAX_PROCESSES][3];

static uint64_t empty_sem = 0;
static uint64_t full_sem = 0;
static uint64_t mutex_sem = 0;

static volatile char shared_value = 0;
static volatile int mvar_running = 0;

static const uint64_t reader_colors[MAX_PROCESSES] = {
    CYAN, GREEN, YELLOW, ORANGE, RED,
    BLUE, WHITE, 0xFF00FF, 0x00FFAA, 0xAAFF00,
    0xFFAA00, 0xAA00FF, 0x00AAFF, 0xFF0055, 0x55FF00,
    0x0055FF, 0xFFFF55, 0x55FFFF, 0xFF55FF, 0xAAAAAA
};

static uint64_t string_hash(const char *str) {
    uint64_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

static void sleep_cycles(int ms) {
    for (volatile int i = 0; i < ms * 8000; i++) {
        if ((i & 0x1FFF) == 0) {
            sys_yield();
        }
    }
}

static int str_to_int(char *str) {
    int res = 0;
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }
    return res;
}

static int str_equals(char *a, char *b) {
    int i = 0;
    while (a[i] != 0 && b[i] != 0 && a[i] == b[i]) {
        i++;
    }
    return a[i] == b[i];
}

static void int_to_str(int num, char *str) {
    int i = 0;
    if (num == 0) {
        str[i++] = '0';
    } else {
        char tmp[10];
        while (num > 0) {
            tmp[i++] = '0' + (num % 10);
            num /= 10;
        }
        for (int j = 0; j < i; j++) {
            str[j] = tmp[i - j - 1];
        }
    }
    str[i] = 0;
}

static int get_random_sleep(int max_ms) {
    static uint32_t seed = 12345;
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return (seed % max_ms) + 5;
}

static void stop_mvar_processes(void) {
    mvar_running = 0;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (writer_pids[i] > 0) {
            sys_kill_process(writer_pids[i]);
            writer_pids[i] = 0;
        }
        if (reader_pids[i] > 0) {
            sys_kill_process(reader_pids[i]);
            reader_pids[i] = 0;
        }
    }

    if (empty_sem != 0) {
        sys_sem_close(empty_sem);
        empty_sem = 0;
    }
    if (full_sem != 0) {
        sys_sem_close(full_sem);
        full_sem = 0;
    }
    if (mutex_sem != 0) {
        sys_sem_close(mutex_sem);
        mutex_sem = 0;
    }

    shared_value = 0;
}

int mvar_is_running(void) {
    return mvar_running;
}

void mvar_force_stop(void) {
    stop_mvar_processes();
}

int writer_main(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }

    int id = str_to_int(argv[1]);
    char my_char = 'A' + (id % 26);

    while (mvar_running) {
        sleep_cycles(get_random_sleep(40));

        if (!mvar_running) {
            break;
        }

        sys_sem_wait(empty_sem);
        if (!mvar_running) {
            sys_sem_post(empty_sem);
            break;
        }

        sys_sem_wait(mutex_sem);
        shared_value = my_char;
        sys_sem_post(mutex_sem);
        sys_sem_post(full_sem);
    }

    return 0;
}

int reader_main(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }

    int id = str_to_int(argv[1]);
    uint64_t color = reader_colors[id % MAX_PROCESSES];

    while (mvar_running) {
        sleep_cycles(get_random_sleep(40));

        if (!mvar_running) {
            break;
        }

        sys_sem_wait(full_sem);
        if (!mvar_running) {
            break;
        }

        sys_sem_wait(mutex_sem);
        char value = shared_value;
        sys_sem_post(mutex_sem);
        sys_sem_post(empty_sem);

        sys_write_color(1, &value, 1, color);
    }

    return 0;
}

int mvar(int argc, char **argv) {
    if (argc < 2) {
        sys_write(1, "Usage: mvar <writers> [readers] | mvar stop\n", 45);
        return -1;
    }

    if (str_equals(argv[1], "stop")) {
        stop_mvar_processes();
        sys_write(1, "mvar stopped\n", 13);
        return 0;
    }

    int num_writers_arg = str_to_int(argv[1]);
    int num_readers_arg = argc >= 3 ? str_to_int(argv[2]) : num_writers_arg;

    if (num_writers_arg <= 0 || num_readers_arg <= 0) {
        sys_write(1, "Use positive process counts\n", 28);
        return -1;
    }

    if (num_writers_arg > MAX_PROCESSES || num_readers_arg > MAX_PROCESSES) {
        sys_write(1, "Too many processes\n", 19);
        return -1;
    }

    stop_mvar_processes();

    empty_sem = string_hash(MVAR_EMPTY);
    full_sem = string_hash(MVAR_FULL);
    mutex_sem = string_hash(MVAR_MUTEX);

    if (!sys_sem_create(empty_sem, 1) || !sys_sem_create(full_sem, 0) ||
        !sys_sem_create(mutex_sem, 1)) {
        sys_write(1, "Could not create semaphores\n", 28);
        stop_mvar_processes();
        return -1;
    }

    mvar_running = 1;

    for (int i = 0; i < num_writers_arg; i++) {
        int_to_str(i, writer_id_args[i]);
        writer_args[i][0] = "writer";
        writer_args[i][1] = writer_id_args[i];
        writer_args[i][2] = NULL;
        writer_pids[i] = sys_create_process("writer", 1, 0, (void *) writer_main, writer_args[i]);
        if (writer_pids[i] <= 0) {
            stop_mvar_processes();
            sys_write(1, "Could not create writer\n", 24);
            return -1;
        }
    }

    for (int i = 0; i < num_readers_arg; i++) {
        int_to_str(i, reader_id_args[i]);
        reader_args[i][0] = "reader";
        reader_args[i][1] = reader_id_args[i];
        reader_args[i][2] = NULL;
        reader_pids[i] = sys_create_process("reader", 1, 0, (void *) reader_main, reader_args[i]);
        if (reader_pids[i] <= 0) {
            stop_mvar_processes();
            sys_write(1, "Could not create reader\n", 24);
            return -1;
        }
    }

    sys_write(1, "\nmvar: cada lector imprime con distinto color. Use mvar stop para detener.\n", 72);
    return 0;
}
