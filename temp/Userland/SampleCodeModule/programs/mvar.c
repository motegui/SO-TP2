#include <usyscalls.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <programs.h>

#define MAX_PROCESSES 20

static int writer_pids[MAX_PROCESSES];
static int reader_pids[MAX_PROCESSES];
static char writer_id_args[MAX_PROCESSES][5];
static char reader_id_args[MAX_PROCESSES][5];
static char reader_total_args[MAX_PROCESSES][5];
static char *writer_args[MAX_PROCESSES][3];
static char *reader_args[MAX_PROCESSES][4];

// Semáfores para sincronización
// filled_count: indica si la variable tiene un valor (0=vacía, 1=llena)
// mutex: exclusión mutua para acceder a la variable
static int64_t filled_count = -1;
static int64_t mutex = -1;

// Variable compartida y su valor actual
volatile static char shared_value = 0;
volatile static bool has_value = false;

static void sleep_cycles(int ms) {
    for (volatile int i = 0; i < ms * 10000; i++) {
        if ((i & 0x3FFF) == 0) {
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
        for (int j = 0; j < i; j++) str[j] = tmp[i - j - 1];
    }
    str[i] = 0;
}

static int get_random_sleep(int max_ms) {
    static uint32_t seed = 12345;
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return (seed % (max_ms + 1)) + 1;
}

// Escritor: espera activa aleatoria, luego espera a que variable esté vacía, escribe único valor
int writer_main(int argc, char **argv) {
    if (argc < 2) return -1;
    
    int id = str_to_int(argv[1]);
    char my_char = 'A' + (id % 26);  // A, B, C, etc.
    
    while (1) {
        // Espera activa aleatoria
        sleep_cycles(get_random_sleep(50));
        
        // Espera a que la variable esté vacía (semáforo filled_count = 0)
        sys_sem_wait(mutex);
        while (has_value) {
            sys_sem_post(mutex);
            sleep_cycles(10);
            sys_sem_wait(mutex);
        }
        
        // Escribe el valor
        shared_value = my_char;
        has_value = true;
        sys_sem_post(mutex);
        
        // Post al semáforo filled_count para notificar a lectores
        sys_sem_post(filled_count);
    }
    
    return 0;
}

// Lector: espera activa aleatoria, luego espera a que variable tenga valor, consume e imprime
int reader_main(int argc, char **argv) {
    if (argc < 3) return -1;
    
    while (1) {
        // Espera activa aleatoria
        sleep_cycles(get_random_sleep(50));
        
        // Espera a que la variable tenga un valor (filled_count > 0)
        sys_sem_wait(filled_count);
        
        // Obtiene acceso a la variable
        sys_sem_wait(mutex);
        char value = shared_value;
        has_value = false;
        sys_sem_post(mutex);
        
        // Imprime el valor
        sys_write(1, &value, 1);
    }
    
    return 0;
}

static void stop_mvar_processes() {
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

    if (filled_count >= 0) {
        sys_sem_close(filled_count);
        filled_count = -1;
    }
    if (mutex >= 0) {
        sys_sem_close(mutex);
        mutex = -1;
    }

    shared_value = 0;
    has_value = false;
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
    
    // Crear semáforos usando handles retornados
    filled_count = sys_sem_create(0, 0);  // Inicialmente vacía
    mutex = sys_sem_create(0, 1);          // Mutex inicialmente liberado

    if (filled_count < 0 || mutex < 0) {
        sys_write(1, "Could not create semaphores\n", 28);
        return -1;
    }
    
    // Crear escritores en background
    for (int i = 0; i < num_writers_arg; i++) {
        int_to_str(i, writer_id_args[i]);
        writer_args[i][0] = "writer";
        writer_args[i][1] = writer_id_args[i];
        writer_args[i][2] = NULL;
        writer_pids[i] = sys_create_process("writer", 1, 0, (void *)writer_main, writer_args[i]);
    }
    
    // Crear lectores en background
    for (int i = 0; i < num_readers_arg; i++) {
        int_to_str(i, reader_id_args[i]);
        int_to_str(num_readers_arg, reader_total_args[i]);
        reader_args[i][0] = "reader";
        reader_args[i][1] = reader_id_args[i];
        reader_args[i][2] = reader_total_args[i];
        reader_args[i][3] = NULL;
        reader_pids[i] = sys_create_process("reader", 1, 0, (void *)reader_main, reader_args[i]);
    }
    
    // El proceso principal termina inmediatamente
    return 0;
}
