#include <stdint.h>
#include <usyscalls.h>

int loop_a_main(int argc, char **argv) {
    while (1) {
        unsigned char c = get_char_no_block();
        if (c == 0x01) {
            sys_write(1, "Saliendo loop_a\n", 16);
            sys_exit_process();
        }

        for (volatile int i = 0; i < 100000000; i++);

        sys_write(1, "A", 1);
    }
    return 0;
}
