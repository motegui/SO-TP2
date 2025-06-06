/* sampleCodeModule.c */
#include <userio.h>
#include <usyscalls.h>
#include <colors.h>
#include <shell.h>
#include <stddef.h>

void shell();  // Declaración del entry point de la shell

int main() {
    char *args[] = { "sh", NULL };
    int pid = sys_create_process("sh", 1, 1, &shell, args);
    sys_wait_pid(pid);
    return 0;
}
