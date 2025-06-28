#include <usyscalls.h>
#include <programs.h>

int wc(char **args) {
    print_char('\n');

    int c;
    int counter = 0;

    while ((c = get_char()) != EOF) {
        if (c == 0x01) {
            break;
        }

        if (c == '\n') {
            print_char('\n');
            counter++;
        } else if (c > 20 && c < 127) {
            print_char(c);
        }
    }

    printf("\nTotal lines: %d", counter);
    return 0;
}