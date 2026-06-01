#include <programs.h>
#include <userio.h>

static int is_vowel(char c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
           c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U';
}

int filter(int argc, char **argv) {
    (void) argc;
    (void) argv;

    int c;
    while ((c = get_char()) != EOF) {
        if (c == '\n' || !is_vowel((char)c)) {
            print_char((char)c);
        }
    }

    return 0;
}
