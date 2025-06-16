#ifndef TEST_UTIL_H
#define TEST_UTIL_H
#include <stdint.h>

#include <stdint.h>
#include "usyscalls.h"
#include <stddef.h>

// Random usando ticks
uint32_t GetUniform(uint32_t max);

// Verifica que todos los bytes en addr sean iguales a val
int memcheck(void *addr, char val, uint32_t size);
// Convierte string a entero (solo positivo)
int satoi(const char *str);

// Wrappers de malloc/free usando syscalls
void *malloc(size_t size);

void free(void *ptr);

// Busy wait
void bussy_wait(uint64_t n);

// Endless loop
void endless_loop();
// Endless loop con print de PID
void endless_loop_print(uint64_t wait);
#endif // TEST_UTIL_H