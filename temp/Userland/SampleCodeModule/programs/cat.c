#include <programs.h>
#include <userio.h>
#include <usyscalls.h>

int cat(char **args) {
    (void) args;

    int c;
    while ((c = get_char()) != EOF) {
        char output = (char)c;
        if ((int64_t)sys_write(1, &output, 1) <= 0) {
            break;
        }
    }

    return 0;
}
