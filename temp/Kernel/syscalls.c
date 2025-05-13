#include <stdint.h>
#include <videodriver.h>
#include <defs.h>
#include <syscalls.h>
#include <keyboard.h>
#include <clock.h>
#include <sound.h>
#include <time.h>

extern const uint64_t registers[17];

void syscallHandler(uint64_t id, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    switch(id) {
        case 0:
            sys_read(arg0, arg1, arg2);
            break;
        case 1:
            sys_write(arg0, arg1, arg2);
            break;
        case 2:
            sys_write_color(arg0, arg1, arg2, arg3);
            break;
        case 3:
            sys_get_registers(arg0);
            break;
        case 4:
            sys_get_time(arg0);
            break;
        case 5:
            sys_get_date(arg0);
            break;
        case 6:
            sys_clear_screen();
            break;
        case 7:
            sys_draw_rect(arg0, arg1, arg2, arg3, arg4);
            break;
        case 8:
            sys_play_sound(arg0, arg1, arg2);
            break;
        case 9:
            sys_get_screensize(arg0, arg1);
            break;
        case 10:
            sys_toggle_cursor();
            break;
        case 11:
            sys_get_ticks(arg0);
            break;
        case 12:
            sys_write_place(arg0, arg1, arg2, arg3, arg4);
            break;
        case 13:
            sys_draw_image(arg0, arg1, arg2);
            break;
    }
    //ver de agregar excepción si no existe el id
}


static int64_t sys_read(uint64_t fd, uint64_t buffer, uint64_t length) {
    if (fd != STDIN) 
        return -1;
    int i = 0;
    char c;
    char * buff = (char *) buffer;
    while(i < length && (c = getChar()) != 0) {
        buff[i] = c;
        i++;
    }
    return i;
}

static void sys_write(uint64_t fd, uint64_t buffer, uint64_t length) {
    if (fd == STDOUT) {
        printStringN((char *) buffer, length);
    } else if (fd == STDERR) {
        printStringNColor((char *) buffer, length, RED);
    }
}

static void sys_write_place(uint64_t fd, uint64_t buffer, uint64_t length, uint64_t x, uint64_t y) {
    if (fd == STDOUT) {
        printStringPlace((char *) buffer, (int) x, (int) y, WHITE);
    } else if (fd == STDERR) {
        printStringPlace((char *) buffer, (int) x, (int) y, RED);
    }
}

static void sys_write_color(uint64_t fd, uint64_t buffer, uint64_t length, uint64_t color) {
    if (fd == STDOUT || fd == STDERR) {
        Color c;
        c.r = (char) color;
        c.g = (char) (color >> 8);
        c.b = (char) (color >> 16);
        printStringNColor((char *) buffer, length, c);
    }
}

static void sys_get_registers(uint64_t regsBuff) {
    for(int i = 0; i < 17; i++) {
        ((uint64_t *)regsBuff)[i] = registers[i];
    }
}

static void sys_get_time(uint64_t buffer) {
    timeToStr((char *) buffer);
}

static void sys_get_date(uint64_t buffer) {
    dateToStr((char *) buffer);
}

static void sys_clear_screen() {
    clearScreen();
}

static void sys_draw_rect(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color) {
    drawRect( (int) x, (int) y, (int) width, (int) height, (int) color );
}

static void sys_play_sound(uint64_t freq, uint64_t duration, uint64_t waitAfter) {
    playNote((int) freq, (int) duration, (int) waitAfter);
}

static void sys_get_screensize(uint64_t width, uint64_t height) {
    uint16_t * w = (uint16_t *) width;
    uint16_t * h = (uint16_t *) height;
    *w = getWidth();
    *h = getHeight();
}

static void sys_toggle_cursor() {
    toggleCursor();
}

static void sys_get_ticks(uint64_t ticks) {
    uint32_t * t = (uint32_t *) ticks;    
    *t = ticks_elapsed();
}

static void sys_draw_image(uint64_t image, uint64_t width, uint64_t height) {
    drawImage((const unsigned long int *) image, (int) width, (int) height);
}