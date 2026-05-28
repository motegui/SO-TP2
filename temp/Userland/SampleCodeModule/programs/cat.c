#include <usyscalls.h>
#include <colors.h>
#include "userio.h"
#include "programs.h"

#define STDIN 0
#define STDOUT 1
#define SIZE 4096
#define EOF -1

int cat(char ** args) {
    char buff[SIZE] = {0};
    char c;
    int i = 0;
    int stdin_is_pipe = (sys_get_io_flags() & 1) != 0;

    if (!stdin_is_pipe) {
        printColorChar('\n', WHITE);
    }

    while ((c = get_char()) != (char) EOF) {
        if (c == '\b') {
            if (i > 0) {
                i--;
                if (!stdin_is_pipe) {
                    printColorChar('\b', WHITE);
                }
            }
        } else if (c == '\n') {
            if (i < SIZE - 1) {
                buff[i++] = c;
            }
            if (!stdin_is_pipe) {
                printColorChar('\n', WHITE);
            }
        } else if (c > 20 && c < 127 && i < SIZE - 1) {
            buff[i++] = c;
            if (!stdin_is_pipe) {
                printColorChar(c, WHITE);
            }
        }else if (c == 0x01) {
            sys_exit_process();
        }
    }

    if (!stdin_is_pipe) {
        printColorChar('\n', WHITE);
    }
    for (int j = 0; j < i; j++) {
        print_char(buff[j]);
    }

    return 0;
}
