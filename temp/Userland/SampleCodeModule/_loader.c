/* _loader.c */
#include <stdint.h>
#include <usyscalls.h>
#include <colors.h>


extern char bss;
extern char endOfBinary;

int main();

void * memset(void * destiny, int32_t c, uint64_t length);

int _start() {
	printColor("start 1\n", RED);

	//Clean BSS
	memset(&bss, 0, &endOfBinary - &bss);
	printColor("start 2\n", RED);

	return main();

}


void * memset(void * destiation, int32_t c, uint64_t length) {
		printColor("memset 1\n", RED);

	uint8_t chr = (uint8_t)c;
			printColor("memset 2\n", RED);

	char * dst = (char*)destiation;
		printColor("memset 3\n", RED);

	while(length--){
		dst[length] = chr;
	}
		printColor("memset 4\n", RED);

	return destiation;	
}
