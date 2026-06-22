#include <programs.h>
#include <userio.h>

int head(char **args) {
    (void) args;

    int c;
    int lines = 0;

    while ((c = get_char()) != EOF) {
        print_char((char)c);

        if (c == '\n') {
            lines++;

            if (lines == 10) {
                break;
            }
        }
    }

    return 0;
}