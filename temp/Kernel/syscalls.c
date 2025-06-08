#include <stdint.h>
#include <videodriver.h>
#include <defs.h>
#include <syscalls.h>
#include <keyboard.h>
#include <clock.h>
#include <sound.h>
#include <time.h>
#include <process_manager.h>
#include <scheduler.h>
#include <mm_manager.h>

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
        case 14:
            sys_create_process(arg0, arg1, arg2, arg3,arg4);
            break;
        case 15:
            sys_exit_process();
            break;
        case 16:
             sys_get_pid();
             break;
        case 17:
            sys_list_processes((char *) arg0, (uint64_t) arg1);
            break;
        case 18:
            sys_kill_process((int) arg0);
            break;
        case 19:
            sys_nice_process(arg0, arg1); //change priority
            break;
        case 20:
            sys_block_process(arg0);
            break;
        case 21:
            sys_unblock_process(arg0);
            break;
        case 22:
            sys_yield();
            break;
        case 23:
            sys_wait_for_children(); //done pero habria que chequearla
            break;
       case 24:
            return sys_malloc(arg0);
        case 25:
            return sys_free((void *) arg0);     // Liberar memoria (arg0 = ptr) //puntero a chequear
            break;
        case 26:
            sys_get_mem_status((size_t *) arg0, (size_t *) arg1);
            break;
        case 27:
            return sys_sem_create(arg0, arg1);
        case 28:
            return sys_sem_open(arg0);      
        case 29:
            return sys_sem_close(arg0);        
        case 30:
            return sys_sem_wait(arg0);       
        case 31:
            return sys_sem_post(arg0);    
        case 32:
            return sys_create_named_pipe((char*) (arg0));
        case 33:
            return sys_read_pipe((int) arg0, (char *) arg1, (int) arg2);
        case 34:
            return sys_write_pipe((int) arg0, (char*)arg1, (int)arg2);
        case 35:
            return (int64_t) sys_wait_pid(arg0);
        }
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

static int64_t sys_create_process(uint64_t name, uint64_t priority, uint64_t foreground, uint64_t entry_point, uint64_t args) {
    PCB *pcb = create_process(name, get_current_process()->pid, priority, foreground, (void *) entry_point, args);
    if (!pcb) return -1;
    printStringNColor("[sys] sys pid:\n", 24, (Color){100, 100, 100});
    printIntLn(pcb->pid);
    return pcb->pid;
}

static void sys_get_pid() {
    get_pid();
}

static void sys_list_processes(char *buffer, uint64_t length) {
    list_processes(buffer, length);
}

static void sys_kill_process(int pid) {
    kill_process(pid);
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


static void sys_nice_process(uint64_t pid, uint64_t new_priority){
    nice_process((int)pid, (int)new_priority);
}

static void sys_block_process(uint64_t pid){
    block_process((int)pid);
}

static void sys_unblock_process(uint64_t pid){
    unblock_process((int)pid);
}

// el proceso actual renuncia al CPU
static void sys_yield() {
    yield();
}

static void sys_exit_process() {
    exit_process();
}


static void sys_wait_for_children(){
    wait_for_children();
}

static uint64_t sys_malloc(uint64_t size) {
    return (uint64_t) allocMemory(globalMemoryManager, (uint64_t) size);
}

static int64_t sys_free(uint64_t ptr) {
    return freeMemory(globalMemoryManager, (void *) ptr);
}

static void sys_get_mem_status(uint64_t *used, uint64_t *free) {
    getMemoryStatus(globalMemoryManager, used, free);
}

static int64_t sys_sem_create(uint64_t  semName, uint64_t  in_value){
    return (int64_t) semCreate((char*) semName, (int) in_value);
}
static int64_t sys_sem_open(uint64_t  semName) {
    return (int64_t) semOpen((char *) semName);
}

static int64_t sys_sem_close(uint64_t  semName) {
    return (int64_t) semClose((char *) semName);
}

static int64_t sys_sem_wait(uint64_t  semName) {
    return (int64_t) semWait((char *) semName);
}

static int64_t sys_sem_post(uint64_t  semName) {
    return (int64_t) semPost((char *) semName);
}

static uint64_t sys_create_named_pipe(char * name){
    return (uint64_t) pipe_open(name);
}

static uint64_t sys_read_pipe(int pipe_id, char *buffer, int count) {
    return (uint64_t) pipe_read(pipe_id, buffer, count); // delegás al sistema de pipes
}

static uint64_t sys_write_pipe(int pipe_id, char *buffer, int count){
    return (uint64_t) pipe_write(pipe_id, buffer, count);
}

static int64_t sys_wait_pid(uint64_t pid){
    printStringColor("[KERNEL] sys_wait_pid recibiendo pid = ", WHITE);
    printIntLn(pid);
    return (int64_t) waitpid((int)pid);
}