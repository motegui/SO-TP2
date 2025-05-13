/* sampleCodeModule.c */
#include <userio.h>
#include <usyscalls.h>
#include <colors.h>
#include <shell.h>
#include <pong.h>

int main() {
	// Choose between terminal or pong
	printColor("Welcome to HomerOS. Choose between terminal or pong (t/p): \n\n", GREEN);

	unsigned char c;
	do {
		c = getChar();
	} while (c != 't' && c != 'p');

	getChar();

	if (c == 'p')
		pong();

	sys_clear_screen();

	shell();

	return 0;
}