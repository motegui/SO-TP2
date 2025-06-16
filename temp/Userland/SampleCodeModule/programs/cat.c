#include <usyscalls.h>
#include <programs.h>

int cat(int argc, char **argv){
    char c;
    while (sys_read(0, &c, 1) == 1) {
        sys_write(1, &c, 1);
    }
    return 0;
}