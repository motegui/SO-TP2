#include <programs.h>
#include <userio.h>

int wc(char **args) {
    (void) args;

    int c;
    int counter = 0;

    while ((c = get_char()) != EOF) {
        if (c == '\n') {
            counter++;
        }
    }

    printf("total lines: %d\n", counter);
    return 0;
}
