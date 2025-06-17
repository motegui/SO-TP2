#ifndef _VIDEODRIVER_H
#define _VIDEODRIVER_H

#include <colors.h>
#include <stdint.h>

void put_pixel(char r, char g, char b, int x, int y);

void draw_white_line();

void draw_rect(int x, int y, int width, int height, int color);

char get_pixel(int x, int y);

char is_space_empty(int x, int y);

void print_char(char c, int x, int y, Color color);

void print_string_place(char * string, int x, int y, Color color);

void print_string(char * string);

void print_stringN(char * string, uint64_t length);

void print_ln(char * string);

void move_one_line_up();

void print_string_color(char * string, Color color);

void print_string_N_color(char * buffer, uint64_t length, Color color);

void move_cursor();

void erase_cursor();

void clear_screen();

uint16_t get_height();

uint16_t get_width();

void toggle_cursor();

void draw_image(const unsigned long int * image, int width, int height);

#endif