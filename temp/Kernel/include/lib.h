#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * lib_memset(void * destination, int32_t character, uint64_t length);
void * lib_memcpy(void * destination, const void * source, uint64_t length);
void lib_strncpy(char * dest, const char * src, uint64_t length);
int lib_strcmp(const char *str1, const char *str2);
char *lib_strchr(const char *s, int c);

char *cpuVendor(char *result);

/* Used for sound */
void outb(int memoryMap, int output); 
int inb(int memoryMap);

#endif