#ifndef U_STRINGS_H
#define U_STRINGS_H

#include <stdint.h>

void strcpy(char * dest, char * src);

int strncmp(char * str1, char * str2, int length);

uint64_t atoi(char * str);

int strtoi(char * buffer, int * i);

void intToStr(int num, char* str);

void int_to_hex(uint64_t num, char* hex);

unsigned int strlen(char* str);

#endif // U_STRINGS_H