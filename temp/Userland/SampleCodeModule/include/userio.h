#ifndef USERIO_H
#define USERIO_H

#include <stdint.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

void print(char* string);

void printColor(char* string, uint64_t color);

void printChar(char c);

void printColorChar(char c, uint64_t color);

char getChar();

void putChar(char c);

unsigned int strlen(char *str);

void printRegs();

void printf(char *format, ...);

void printfColor(char *format, uint64_t color, ...);

// int scanf(char *format, ...);

void strcpy(char * dest, char * src);

int strncmp(char * str1, char * str2, int length);

void fillRegisters();

char * getTime();

char * getDate();

#endif