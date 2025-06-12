/* _loader.c */
#include <stdint.h>
#include <usyscalls.h>
#include <colors.h>


extern char bss;
extern char endOfBinary;

int main();

void * memset(void * destiny, int32_t c, uint64_t length);

int _start() {

	memset(&bss, 0, &endOfBinary - &bss);

	return main();

}


void * memset(void * destiation, int32_t c, uint64_t length) {

	uint8_t chr = (uint8_t)c;

	char * dst = (char*)destiation;

    for (uint64_t i = 0; i < length; i++) {
        dst[i] = chr;
    }

	return destiation;	
}
