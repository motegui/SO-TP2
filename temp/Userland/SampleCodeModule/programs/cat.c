#include <usyscalls.h>
#include <programs.h>

<<<<<<< Updated upstream
int cat(int argc, char **argv){
    char c;
    while (sys_read(0, &c, 1) == 1) {
        sys_write(1, &c, 1);
    }
=======
#define STDIN 0
#define STDOUT 1
#define SIZE 128

int cat(char **args) {
    char buff[SIZE] = {0};
    char c;
    int i = 0;

    while (sys_read(STDIN, &c, 1) == 1) {
        if (c == '\b') {
            if (i > 0) {
                i--;
                sys_write(STDOUT, "\b \b", 3);
            }
        } else if (c == '\n') {
            sys_write(STDOUT, &c, 1);
            sys_write(STDOUT, buff, i);
            sys_write(STDOUT, "\n", 1);
            i = 0;
        } else if (c >= 32 && c < 127 && i < SIZE) {
            buff[i++] = c;
            sys_write(STDOUT, &c, 1);
        }
    }

    sys_write(STDOUT, "\n[Saliendo de cat]\n", 20);
>>>>>>> Stashed changes
    return 0;
}