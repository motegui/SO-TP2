#include <stdint.h>
#include "usyscalls.h"
#include <stddef.h>

// Random usando ticks
uint32_t GetUniform(uint32_t max) {
    if (max == 0) return 0;
    return (uint32_t)(sys_get_ticks() % (max + 1));
}

// Verifica que todos los bytes en addr sean iguales a val
int memcheck(void *addr, char val, uint32_t size) {
    unsigned char *p = (unsigned char *)addr;
    for (uint32_t i = 0; i < size; i++) {
        if (p[i] != (unsigned char)val)
            return 0;
    }
    return 1;
}

// Convierte string a entero (solo positivo)
int satoi(const char *str) {
    int res = 0;
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }
    return res;
}

// Wrappers de malloc/free usando syscalls
void *malloc(size_t size) {
    return sys_malloc(size);
}

void free(void *ptr) {
    sys_free(ptr);
}

// Busy wait
void bussy_wait(uint64_t n) {
    for (volatile uint64_t i = 0; i < n; i++) {
        sys_yield();
    }
}

// Endless loop
void endless_loop() {
    while (1) {
        sys_yield(); // Así no bloqueás el sistema si hay multitarea
    }
}

// Endless loop con print de PID
void endless_loop_print(uint64_t wait) {
    int64_t pid = sys_get_pid();
    char buffer[32];
    while (1) {
        // Convierte el PID a string para imprimirlo
        int len = 0, tmp = pid;
        if (tmp == 0) {
            buffer[len++] = '0';
        } else {
            while (tmp > 0 && len < 31) {
                buffer[len++] = '0' + (tmp % 10);
                tmp /= 10;
            }
        }
        // Invierte el string
        for (int i = 0; i < len / 2; i++) {
            char c = buffer[i];
            buffer[i] = buffer[len - 1 - i];
            buffer[len - 1 - i] = c;
        }
        buffer[len++] = '\n';
        sys_write(1, buffer, len);
        bussy_wait(wait);
        sys_yield();
    }
}