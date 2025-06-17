#include <usyscalls.h>

#define STDIN 0
#define STDOUT 1
#define SIZE 128  // tamaño del buffer de línea

int cat(int argc, char **argv) {
    char buff[SIZE];
    char c;
    int i = 0;

    while (1) {
        int read_res = sys_read(STDIN, &c, 1);

        if (read_res <= 0) {
            break;  // EOF o error
        }

        if (c == 4) {  // Ctrl+D (EOF)
            break;
        }

        if (c == '\b' || c == 127) {
            if (i > 0) {
                i--;
                sys_write(STDOUT, "\b \b", 3);
            }
        } else if (c == '\n') {
            sys_write(STDOUT, "\n", 1);
            sys_write(STDOUT, buff, i);
            sys_write(STDOUT, "\n", 1);
            i = 0;
        } else if (c >= 32 && c < 127) {
            if (i < SIZE - 1) {
                buff[i++] = c;
                sys_write(STDOUT, &c, 1);
            }
        }
    }

    return 0;
}
