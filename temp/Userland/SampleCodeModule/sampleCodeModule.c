/* sampleCodeModule.c */
#include <userio.h>
#include <usyscalls.h>
#include <colors.h>
#include <shell.h>
#include <pong.h>
#include <stddef.h>

int loop_a_main(int argc, char **argv);

int main() {
    char *args[] = { "loop_a", NULL };
    int pid = sys_create_process("loop_a", 1, 1, loop_a_main, args);
    sys_wait_pid(pid);  // ✅ Esperar que termine el proceso hijo
    return 0;
}