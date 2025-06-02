#include <usyscalls.h>
#include <programs.h>

int wc(int argc, char **argv){
    char c;
    int lines = 0;
    while (sys_read(0, &c, 1) == 1) {
        if (c == '\n')
            lines++;
    }
    sys_write(1, "Cantidad de lineas: ", 20);
    
    // Convertir lines a string simple (solo para números chicos)
    char buf[12];
    int i = 10;
    buf[11] = 0;
    if (lines == 0) {
        buf[i--] = '0';
    } else {
        int n = lines;
        while (n > 0 && i >= 0) {
            buf[i--] = '0' + (n % 10);
            n /= 10;
        }
    }
    sys_write(1, buf + i + 1, 10 - i);
    sys_write(1, "\n", 1);
    return 0;
}