#include <usyscalls.h>
#include <programs.h>

#include <userio.h>

#define SIZE 512

int is_vowel(char c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
           c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U';
}

int filter(char **args) {
    char buffer[SIZE] = {0};
    char c;
    int i = 0;

    print_char('\n');

    while ((c = get_char()) != EOF) {
        if (c == 0x01) {
            break;
        }

        if (c == '\b') {
            if (i > 0) {
                i--;
                print_char('\b');
            }
        } else if (c == '\n') {
            print_char('\n');

            // Imprimir solo las vocales
            for (int j = 0; j < i; j++) {
                if (is_vowel(buffer[j])) {
                    print_char(buffer[j]);
                }
            }

            print_char('\n');
            i = 0;
        } else if (c > 20 && c < 127 && i < SIZE - 1) {
            buffer[i++] = c;
            print_char(c);
        }
    }

    return 0;
}