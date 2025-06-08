#include <stdint.h>
#include <usyscalls.h> 

int loop_a_main(int argc, char **argv) {
    sys_write(1, "Start\n", 6);


    return 0;
}
