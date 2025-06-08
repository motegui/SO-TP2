/* sampleCodeModule.c */
#include <userio.h>
#include <usyscalls.h>
#include <colors.h>
#include <shell.h>
#include <pong.h>
#include <stddef.h>

int loop_a_main(int argc, char **argv);

int main(int argc, char **argv) {

    //loop_a_main(argc, argv);
    shell();
    return 0;
}