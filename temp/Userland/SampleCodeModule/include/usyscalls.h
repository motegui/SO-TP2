#ifndef USYSCALLS_H
#define USYSCALLS_H

#include <stdint.h>

uint64_t sys_read(unsigned int fd, char* buffer, unsigned int size);

uint64_t sys_write(unsigned int fd, const char* buffer, unsigned int size);

uint64_t sys_write_color(unsigned int fd, const char* buffer, unsigned int size, uint64_t color);

uint64_t sys_get_regs(uint64_t * regsBuff);

uint64_t sys_get_time(char *buffer);

uint64_t sys_get_date(char *buffer);

uint64_t sys_clear_screen();

uint64_t sys_draw_rectangle(int x, int y, int width, int height, int color);

uint64_t sys_play_sound(int freq, int duration, int waitAfter);

uint64_t sys_get_screen_size(uint16_t * width, uint16_t * height);

uint64_t sys_toggle_cursor();

uint64_t sys_get_ticks(uint32_t * ticks);

uint64_t sys_write_place(unsigned int fd, const char* buffer, unsigned int size, int x, int y);

uint64_t sys_draw_image(const unsigned long * image, int width, int height);

#endif