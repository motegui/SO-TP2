#include <stdint.h>
#include <usyscalls.h> 

int loop_a_main(int argc, char **argv) {
    sys_write(1, "Start\n", 6);

    while (1) {
        sys_write(1, "A", 1);
        sys_yield();
    }
    return 0;
}
