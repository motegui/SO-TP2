#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyboard_handler();

char get_char();

char get_char_no_block();

extern unsigned char getKey();
int is_keyboard_buffer_empty();
char dequeue_keyboard_char();
#endif