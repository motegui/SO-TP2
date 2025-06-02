#include <usyscalls.h>
#include <programs.h>

int is_vowel(char c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
           c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U';
}

int filter(int argc, char **argv){
    char c;
    while (sys_read(0, &c, 1) == 1) {
        if (!is_vowel(c)) {
            sys_write(1, &c, 1);
        }
    }
    return 0;
}