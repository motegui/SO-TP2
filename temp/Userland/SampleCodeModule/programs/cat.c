#include <usyscalls.h>
#include "userio.h"
#include "programs.h"

#define STDIN 0
#define STDOUT 1
#define SIZE 128
#define EOF -1

int cat(char ** args) {
    char buff[SIZE] = {0};
    char c;
    int i = 0;
    print_char('\n');

    while ((c = get_char()) != (char) EOF) {
        if (c == '\b') {
            if (i > 0) {
                i--;
                print_char('\b');
            }
        } else if (c == '\n') {
            print_char('\n');
            for (int j = 0; j < i; j++) {
                print_char(buff[j]);
            }
            i = 0;
            print_char('\n');
        } else if (c > 20 && c < 127 && i < SIZE - 1) {
            buff[i++] = c;
            print_char(c);
        }else if (c == 0x01) {
            sys_exit_process();
        }
    }

    return 0;
}