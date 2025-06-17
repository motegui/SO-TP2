#include <videodriver.h>  // o donde esté printChar

int loop_a_main(int argc, char **argv) {
    while (1) {
        print_char('A', 10, 10, (Color){255, 255, 255});
    }
    return 0;
}