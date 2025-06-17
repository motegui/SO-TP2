#include <stdint.h>
#include <usyscalls.h>

int loop_a_main(int argc, char **argv) {
    while (1) {
        // Chequea si hay una tecla presionada (no bloqueante)
        unsigned char c = getCharNoBlock();
        if (c == 0x01) {  // ESC
            sys_write(1, "Saliendo loop_a\n", 16);
            sys_exit_process();  // O la syscall que uses para finalizar el proceso
        }

        // Espera activa
        for (volatile int i = 0; i < 100000000; i++);

        // Print opcional
        sys_write(1, "A", 1);
    }
    return 0;
}
