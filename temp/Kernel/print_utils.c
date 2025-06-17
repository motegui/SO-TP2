#include "print_utils.h"
#include <stdarg.h>   // para va_list, va_start, va_arg, va_end

int int_to_str(int value, char *str) {
    char temp[12];
    int i = 0, j = 0;
    int is_negative = 0;

    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value > 0) {
        temp[i++] = (value % 10) + '0';
        value /= 10;
    }

    if (is_negative) {
        temp[i++] = '-';
    }

    while (i > 0) {
        str[j++] = temp[--i];
    }

    str[j] = '\0';
    return j;
}

int snprintf(char *buffer, uint64_t size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    uint64_t offset = 0;
    for (const char *p = fmt; *p && offset < size - 1; ++p) {
        if (*p == '%') {
            p++; // Avanzamos al especificador
            if (!*p) break;

            if (*p == 'd') {
                int val = va_arg(args, int);
                char temp[12];
                int len = int_to_str(val, temp);
                for (int i = 0; i < len && offset < size - 1; i++)
                    buffer[offset++] = temp[i];
            } else if (*p == 's') {
                const char *str = va_arg(args, const char*);
                while (*str && offset < size - 1)
                    buffer[offset++] = *str++;
            } else if (*p == 'c') {
                char c = (char)va_arg(args, int);
                if (offset < size - 1)
                    buffer[offset++] = c;
            } else {
                // No reconocido, lo copiamos literalmente
                if (offset < size - 1) buffer[offset++] = '%';
                if (offset < size - 1) buffer[offset++] = *p;
            }
        } else {
            buffer[offset++] = *p;
        }
    }

    buffer[offset] = '\0';
    va_end(args);
    return offset;
}


