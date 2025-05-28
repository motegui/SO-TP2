#include <usyscalls.h>

extern uint64_t sys_call(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

uint64_t sys_read(unsigned int fd, char* buffer, unsigned int size) {
    return sys_call((uint64_t) 0, (uint64_t) fd, (uint64_t) buffer, (uint64_t) size, (uint64_t) 0, (uint64_t) 0);
}

uint64_t sys_write(unsigned int fd, const char* buffer, unsigned int size) {
    return sys_call((uint64_t) 1, (uint64_t) fd, (uint64_t) buffer, (uint64_t) size, (uint64_t) 0, (uint64_t) 0);
}

uint64_t sys_write_color(unsigned int fd, const char* buffer, unsigned int size, uint64_t color) {
    /* uint64_t c = color.r;
    c <<= 2;
    c += color.g;
    c <<= 2;
    c += color.b; */
    return sys_call((uint64_t) 2, (uint64_t) fd, (uint64_t) buffer, (uint64_t) size, color, (uint64_t) 0);
}

uint64_t sys_get_regs(uint64_t * regsBuff) {
    return sys_call((uint64_t) 3, (uint64_t) regsBuff, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0);
}

uint64_t sys_get_time(char * buffer) {
    return sys_call((uint64_t)4, (uint64_t)buffer, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0);
}

uint64_t sys_get_date(char * buffer) {
    return sys_call((uint64_t)5, (uint64_t)buffer, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0);
}

uint64_t sys_clear_screen() {
    return sys_call((uint64_t)6, (uint64_t)0, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0);
}

uint64_t sys_draw_rectangle(int x, int y, int width, int height, int color) {
    return sys_call((uint64_t) 7, (uint64_t) x, (uint64_t) y, (uint64_t) width, (uint64_t) height, (uint64_t) color);
}

uint64_t sys_play_sound(int freq, int duration, int waitAfter) {
    return sys_call((uint64_t) 8, (uint64_t) freq, (uint64_t) duration, (uint64_t) waitAfter, (uint64_t) 0, (uint64_t) 0);
}

uint64_t sys_get_screen_size(uint16_t * width, uint16_t * height) {
    return sys_call((uint64_t) 9, (uint64_t) width, (uint64_t) height, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0);
}

uint64_t sys_toggle_cursor() {
    return sys_call((uint64_t) 10, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0);
}

uint64_t sys_get_ticks(uint32_t * ticks) {
    return sys_call((uint64_t) 11, (uint64_t) ticks, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0, (uint64_t) 0);
}

uint64_t sys_write_place(unsigned int fd, const char* buffer, unsigned int size, int x, int y) {
    return sys_call((uint64_t) 12, (uint64_t) fd, (uint64_t) buffer, (uint64_t) size, (uint64_t) x, (uint64_t) y);
}

uint64_t sys_draw_image(const unsigned long * image, int width, int height) {
    return sys_call((uint64_t) 13, (uint64_t) image, (uint64_t) width, (uint64_t) height, (uint64_t) 0, (uint64_t) 0);
}

uint64_t sys_create_process(char *name, int priority, int foreground, int detached) {
    return sys_call(14, (uint64_t)name, (uint64_t)priority, (uint64_t)foreground, (uint64_t)detached, 0);
}

void sys_exit_process() {
    sys_call(15, 0, 0, 0, 0, 0);
}

uint64_t sys_get_pid() {
    return sys_call(16, 0, 0, 0, 0, 0);
}

void sys_list_processes(char *buffer, uint64_t length) {
    sys_call(17, (uint64_t)buffer, length, 0, 0, 0);
}

void sys_kill_process(int pid) {
    sys_call(18, (uint64_t)pid, 0, 0, 0, 0);
}

void sys_nice_process(uint64_t pid, uint64_t new_priority) {
    sys_call(19, pid, new_priority, 0, 0, 0);
}

void sys_block_process(uint64_t pid) {
    sys_call(20, pid, 0, 0, 0, 0);
}

void sys_unblock_process(uint64_t pid) {
    sys_call(21, pid, 0, 0, 0, 0);
}

void sys_yield() {
    sys_call(22, 0, 0, 0, 0, 0);
}

void sys_wait_for_children() {
    sys_call(23, 0, 0, 0, 0, 0);
}

void *sys_malloc(uint64_t size) {
    return (void *)sys_call(24, size, 0, 0, 0, 0);
}

int64_t sys_free(void *ptr) {
    return sys_call(25, (uint64_t)ptr, 0, 0, 0, 0);
}

void sys_get_mem_status(uint64_t *used, uint64_t *free) {
    sys_call(26, (uint64_t)used, (uint64_t)free, 0, 0, 0);
}
