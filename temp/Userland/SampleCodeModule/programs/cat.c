#include <programs.h>
#include <userio.h>

int cat(char **args) {
    (void) args;

    int c;
    while ((c = get_char()) != EOF) {
        print_char((char)c);
    }

    return 0;
}
