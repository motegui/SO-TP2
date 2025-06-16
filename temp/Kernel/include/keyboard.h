#ifndef KEYBOARD_H
#define KEYBOARD_H

#define BUFFER_SIZE       512
#define MAX_KEY_PRESSED   127
#define LEFT_CTRL         0x1D
#define RELEASE_LEFT_CTRL 0x9D
#define RELEASE_SHIFT     0xAA
#define CAPS_LOCK         0x3A
void keyboard_handler();

char getChar();

extern unsigned char getKey();

char getCharNoBlock();
void initKeyboard();

#endif