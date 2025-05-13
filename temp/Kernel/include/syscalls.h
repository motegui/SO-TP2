#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

void syscallHandler(uint64_t id, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);

static int64_t sys_read(uint64_t fd, uint64_t buffer, uint64_t length);

static void sys_write(uint64_t fd, uint64_t buffer, uint64_t length);

static void sys_write_color(uint64_t fd, uint64_t buffer, uint64_t length, uint64_t color);

static void sys_write_place(uint64_t fd, uint64_t buffer, uint64_t length, uint64_t x, uint64_t y) ;

static void sys_get_registers(uint64_t regsBuff);

static void sys_get_time(uint64_t buffer);

static void sys_get_date(uint64_t buffer);

static void sys_clear_screen();

static void sys_draw_rect(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color);

static void sys_play_sound(uint64_t freq, uint64_t duration, uint64_t waitAfter);

static void sys_get_screensize(uint64_t width, uint64_t height);

static void sys_toggle_cursor();

static void sys_get_ticks(uint64_t ticks);

static void sys_draw_image(uint64_t image, uint64_t width, uint64_t height);

#endif