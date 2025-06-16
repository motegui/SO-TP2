#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyboard_handler();

char getChar();

char getCharNoBlock();

extern unsigned char getKey();

#endif