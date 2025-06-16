#include <usyscalls.h>

#define STDIN 0
#define STDOUT 1
#define SIZE 128  // tamaño del buffer de línea

int cat(char **args) {
    char buff[SIZE] = {0};
    char c;
    int i = 0;

    while (sys_read(STDIN, &c, 1) == 1) {
        if (c == '\b') {  // backspace
            if (i > 0) {
                i--;
                sys_write(STDOUT, "\b \b", 3);  // borra del terminal
            }
        } else if (c == '\n') {
            sys_write(STDOUT, &c, 1);  // imprime newline
            sys_write(STDOUT, buff, i);  // imprime línea entera
            sys_write(STDOUT, "\n", 1);
            i = 0;
        } else if (c >= 32 && c < 127 && i < SIZE) {
            buff[i++] = c;
            sys_write(STDOUT, &c, 1);  // echo del caracter
        }
    }

    return 0;
}
