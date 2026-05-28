#include <usyscalls.h>
#include <programs.h>
#include <userio.h>
#include <colors.h>

#define SIZE 4096

static int is_vowel(char c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
           c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U';
}

int filter(int argc, char **argv) {
    (void) argc;
    (void) argv;

    char buffer[SIZE] = {0};
    int c;
    int i = 0;
    int stdin_is_pipe = (sys_get_io_flags() & 1) != 0;

    while ((c = get_char()) != EOF) {
        if (c == '\b') {
            if (i > 0) {
                i--;
                if (!stdin_is_pipe) {
                    printColorChar('\b', WHITE);
                }
            }
        } else if (c == '\n') {
            if (i < SIZE - 1) {
                buffer[i++] = (char)c;
            }
            if (!stdin_is_pipe) {
                printColorChar('\n', WHITE);
            }
        } else if (c > 20 && c < 127 && i < SIZE - 1) {
            buffer[i++] = (char)c;
            if (!stdin_is_pipe) {
                printColorChar((char)c, WHITE);
            }
        }
    }

    if (!stdin_is_pipe) {
        printColorChar('\n', WHITE);
    }
    for (int j = 0; j < i; j++) {
        if (buffer[j] == '\n') {
            putChar('\n');
        } else if (!is_vowel(buffer[j])) {
            putChar(buffer[j]);
        }
    }

    return 0;
}
