#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include <stdint.h>
#include <stdarg.h>

// Convierte un entero a string. Devuelve la cantidad de caracteres escritos.
int int_to_str(int value, char *str);

// Implementación limitada de snprintf (soporta %d, %s, %c).
int snprintf(char *buffer, uint64_t size, const char *fmt, ...);
char *strdup_kernel(const char *src);

#endif // PRINT_UTILS_H
