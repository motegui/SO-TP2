#ifndef USYSCALLS_H
#define USYSCALLS_H

#include <stdint.h>

// Forward declarations
typedef struct memoryData * MemoryDataPtr;
typedef struct processInfo * ProcessInfoPtr;

uint64_t sys_read(unsigned int fd, char* buffer, unsigned int size);
uint64_t sys_read_no_block(unsigned int fd, char * buffer, unsigned int size);

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

uint64_t sys_get_ticks();

uint64_t sys_write_place(unsigned int fd, const char* buffer, unsigned int size, int x, int y);

uint64_t sys_draw_image(const unsigned long * image, int width, int height);

uint64_t sys_create_process(char *name, int priority, int foreground, void *entry_point, char **args);

void sys_exit_process();

uint64_t sys_get_pid();

void sys_list_processes(char *buffer, uint64_t length);

uint64_t sys_kill_process(int pid);

void sys_nice_process(uint64_t pid, uint64_t new_priority);

uint64_t sys_block_process(uint64_t pid);

uint64_t sys_unblock_process(uint64_t pid);

int64_t sys_free_processes_info(ProcessInfoPtr * info);

void sys_yield();

void sys_wait_for_children();

void *sys_malloc(uint64_t size);

int64_t sys_free(void *ptr);

void sys_get_mem_status(uint64_t *used, uint64_t *free);

int64_t sys_sem_create(uint64_t semName, uint64_t in_value);

int64_t sys_sem_close(uint64_t semName);

int64_t sys_sem_wait(uint64_t semName);

int64_t sys_sem_post(uint64_t semName);

uint64_t sys_wait_pid(uint64_t pid);

uint64_t sys_close_pipe(int pipe_id);

// Additional function declarations
uint64_t sys_mem_data();
ProcessInfoPtr * sys_processes_info();
int64_t sys_change_priority(int pid, int new_priority);
int sys_pipe_open(const char *name);
int sys_pipe_read(int pipe_id, char *buffer, unsigned int count);
int sys_pipe_write(int pipe_id, const char *buffer, unsigned int count);

// Shell helper functions
int has_pipe(char *buffer);
void analyze_piped_command(char *buffer, int count);

#endif
