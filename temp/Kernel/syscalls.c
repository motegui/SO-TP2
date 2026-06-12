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
#include <pipes.h>
#include <interrupts.h>
#include <lib.h>

extern const uint64_t registers[17];

static uint64_t sys_mem_data();
static uint64_t sys_get_io_flags();
static int uses_canonical_terminal_input(PCB *current);

uint64_t syscallHandler(uint64_t id, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    switch(id) {
        case 0:
            return (uint64_t) sys_read(arg0, arg1, arg2, arg3);
        case 1:
            return sys_write(arg0, arg1, arg2);
        case 2:
            sys_write_color(arg0, arg1, arg2, arg3);
            return 0;
        case 3:
            sys_get_registers(arg0);
            return 0;
       	case 4:
			return (uint64_t) sys_get_time(arg0);
		case 5:
			return (uint64_t) sys_get_date(arg0);
        case 6:
            sys_clear_screen();
            return 0;
        case 7:
            sys_draw_rect(arg0, arg1, arg2, arg3, arg4);
            return 0;
        case 8:
            sys_play_sound(arg0, arg1, arg2);
            return 0;
        case 9:
            sys_get_screensize(arg0, arg1);
            return 0;
        case 10:
            sys_toggle_cursor();
            return 0;
        case 11:
            return sys_get_ticks();
        case 12:
            sys_write_place(arg0, arg1, arg2, arg3, arg4);
            return 0;
        case 13:
            sys_draw_image(arg0, arg1, arg2);
            return 0;
        case 14:
            return (uint64_t) sys_create_process(arg0, arg1, arg2, arg3, arg4);
        case 15:
            sys_exit_process();
            return 0;
        case 16: {
             PCB *current = get_current_process();
             return current ? (uint64_t) current->pid : 0;
        }
        case 17:
            sys_list_processes((char *) arg0, (uint64_t) arg1);
            return 0;
        case 18:
            sys_kill_process((int) arg0);
            return 0;
        case 19:
            sys_nice_process(arg0, arg1);
            return 0;
        case 20:
            sys_block_process(arg0);
            return 0;
        case 21:
            sys_unblock_process(arg0);
            return 0;
        case 22:
            sys_yield();
            return 0;
        case 23:
            sys_wait_for_children();
            return 0;
       case 24:
            return sys_malloc(arg0);
        case 25:
            return (uint64_t) sys_free(arg0);
        case 26:
            sys_get_mem_status((size_t *) arg0, (size_t *) arg1);
            return 0;
        case 27:
            return (uint64_t) sys_sem_create(arg0, arg1);
        case 29:
            return (uint64_t) sys_sem_close(arg0);
        case 30:
            return (uint64_t) sys_sem_wait(arg0);
        case 31:
            return (uint64_t) sys_sem_post(arg0);
        case 32:
            return sys_create_named_pipe((char*) (arg0));
        case 33:
            return sys_read_pipe((int) arg0, (char *) arg1, (int) arg2);
        case 34:
            return sys_write_pipe((int) arg0, (char*)arg1, (int)arg2);
        case 35:
            return (uint64_t) sys_wait_pid(arg0);
        case 36:
            return sys_close_pipe((int) arg0);
        case 37:
            *((uint64_t*)arg0) = sys_mem_data();
            return 0;
        case 38:
            return sys_processes_info();
        case 39:
            return (uint64_t) sys_free_processes_info(arg0);
        case 40:
            set_pending_process_io((int) arg0, (int) arg1);
            return 0;
        case 41:
            pipe_shutdown_write((int) arg0);
            return 0;
        case 42:
            return sys_get_io_flags();
        default:
            return 0;
    }
}


int64_t sys_read(uint64_t fd, uint64_t buffer, uint64_t length, uint64_t shouldNotBlock) {
    if (fd != STDIN || length == 0) {
        return -1;
    }

    PCB *current = get_current_process();
    char *buff = (char *)buffer;
    int i = 0;
    static char terminal_line[512];
    static int terminal_line_len = 0;
    static int terminal_line_pos = 0;
    static int terminal_line_owner = -1;

    if (current != NULL && current->stdin_pipe >= 0) {
        return pipe_read(current->stdin_pipe, buff, (unsigned int) length);
    }

    if (uses_canonical_terminal_input(current)) {
        if (terminal_line_owner != current->pid) {
            terminal_line_len = 0;
            terminal_line_pos = 0;
            terminal_line_owner = current->pid;
        }

        while (i < (int) length) {
            if (terminal_line_pos >= terminal_line_len) {
                terminal_line_len = 0;
                terminal_line_pos = 0;

                while (terminal_line_len < (int) sizeof(terminal_line)) {
                    if (shouldNotBlock && is_keyboard_buffer_empty()) {
                        return i;
                    }

                    while (is_keyboard_buffer_empty()) {
                        _hlt();
                    }

                    char c = dequeue_keyboard_char();
                    if ((signed char)c == -1) {
                        if (terminal_line_len == 0) {
                            return i;
                        }
                        break;
                    }

                    if (c == '\b') {
                        if (terminal_line_len > 0) {
                            terminal_line_len--;
                            print_stringN(&c, 1);
                        }
                        continue;
                    }

                    terminal_line[terminal_line_len++] = c;
                    if (c == '\n' || c == '\t' || (c >= ' ' && c < 127)) {
                        print_stringN(&c, 1);
                    }

                    if (c == '\n') {
                        break;
                    }
                }
            }

            if (terminal_line_pos < terminal_line_len) {
                buff[i++] = terminal_line[terminal_line_pos++];
            } else {
                break;
            }
        }

        return i;
    }

    while (i < (int) length) {
        if (shouldNotBlock && is_keyboard_buffer_empty()) {
            return i;
        }

        while (is_keyboard_buffer_empty()) {
            _hlt();
        }

        char c = dequeue_keyboard_char();
        if ((signed char) c == -1) {
            return i;
        }

        buff[i++] = c;
    }

    return i;
}

static int uses_canonical_terminal_input(PCB *current) {
    if (current == NULL || current->name == NULL || !current->foreground || current->stdin_pipe >= 0) {
        return 0;
    }

    return lib_strcmp(current->name, "cat") == 0 ||
           lib_strcmp(current->name, "wc") == 0 ||
           lib_strcmp(current->name, "filter") == 0;
}

static int64_t sys_write(uint64_t fd, uint64_t buffer, uint64_t length) {
    PCB *current = get_current_process();

    if (fd == STDOUT && current != NULL && current->stdout_pipe >= 0) {
        return pipe_write(current->stdout_pipe, (char *) buffer, (unsigned int) length);
    }

    if (fd == STDOUT) {
        print_stringN((char *) buffer, length);
        return (int64_t)length;
    } else if (fd == STDERR) {
        print_string_N_color((char *) buffer, length, RED);
        return (int64_t)length;
    }

    return -1;
}

static void sys_write_place(uint64_t fd, uint64_t buffer, uint64_t length, uint64_t x, uint64_t y) {
    if (fd == STDOUT) {
        print_string_place((char *) buffer, (int) x, (int) y, WHITE);
    } else if (fd == STDERR) {
        print_string_place((char *) buffer, (int) x, (int) y, RED);
    }
}

static int64_t sys_free_processes_info(uint64_t info) {
	freeProcessesInfo((processInfo **) info);
	return 0;
}

static void sys_write_color(uint64_t fd, uint64_t buffer, uint64_t length, uint64_t color) {
    PCB *current = get_current_process();

    if (fd == STDOUT && current != NULL && current->stdout_pipe >= 0) {
        pipe_write(current->stdout_pipe, (char *) buffer, (unsigned int) length);
        return;
    }

    if (fd == STDOUT || fd == STDERR) {
        Color c;
        c.r = (char) color;
        c.g = (char) (color >> 8);
        c.b = (char) (color >> 16);
        print_string_N_color((char *) buffer, length, c);
    }
}

static void sys_get_registers(uint64_t regsBuff) {
    for(int i = 0; i < 17; i++) {
        ((uint64_t *)regsBuff)[i] = registers[i];
    }
}

static int64_t sys_get_time(uint64_t buffer) {
	timeToStr((char *) buffer);
	return 0;
}

static int64_t sys_get_date(uint64_t buffer) {
	dateToStr((char *) buffer);
	return 0;
}

static void sys_clear_screen() {
    clear_screen();
}


static void sys_draw_rect(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color) {
    draw_rect( (int) x, (int) y, (int) width, (int) height, (int) color );
}

static void sys_play_sound(uint64_t freq, uint64_t duration, uint64_t waitAfter) {
    playNote((int) freq, (int) duration, (int) waitAfter);
}

static void sys_get_screensize(uint64_t width, uint64_t height) {
    uint16_t * w = (uint16_t *) width;
    uint16_t * h = (uint16_t *) height;
    *w = get_width();
    *h = get_height();
}

static int64_t sys_create_process(uint64_t name, uint64_t priority, uint64_t foreground, uint64_t entry_point, uint64_t args) {
    PCB *parent = get_current_process();
    int parent_pid = parent ? parent->pid : 0;
    PCB *pcb = create_process((const char *)name, parent_pid, priority, foreground, (void *) entry_point, (char **)args);
    if (!pcb) return -1; 
    return pcb->pid;
}

static void sys_list_processes(char *buffer, uint64_t length) {
    list_processes(buffer, length);
}

static void sys_kill_process(int pid) {
    kill_process(pid);
}

static void sys_toggle_cursor() {
    toggle_cursor();
}

static uint64_t sys_get_ticks(void) {
    return (uint64_t) ticks_elapsed();
}

static void sys_draw_image(uint64_t image, uint64_t width, uint64_t height) {
    draw_image((const unsigned long int *) image, (int) width, (int) height);
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

static int64_t sys_sem_create(uint64_t semName, uint64_t in_value){
    return sem_open_named(semName, (int) in_value) != NULL;
}


static int64_t sys_sem_close(uint64_t  semName) {
    return (int64_t) sem_close_named(semName);
}

static uint64_t sys_write_pipe(int pipe_id, char *buffer, int count){
    return (uint64_t) pipe_write(pipe_id, buffer, count);
}

static int64_t sys_wait_pid(uint64_t pid){
    return (int64_t) wait_pid((int)pid);
}

static uint64_t sys_close_pipe(int pipe_id){
    pipe_close(pipe_id);
    return 0;
}

// Add missing sys_* function implementations
static int64_t sys_sem_wait(uint64_t sem_id) {
    return sem_wait_named(sem_id);
}

static int64_t sys_sem_post(uint64_t sem_id) {
    return sem_post_named(sem_id);
}

static uint64_t sys_create_named_pipe(char *name) {
    return pipe_open(name);
}

static uint64_t sys_read_pipe(int pipe_id, char *buffer, int count) {
    return pipe_read(pipe_id, buffer, count);
}

static uint64_t sys_processes_info() {
    PCBNode *curr = get_active_process_list();
    if (!curr) return 0;
    
    int count = 0;
    PCBNode *temp = curr;
    do {
        count++;
        temp = temp->next ? temp->next : get_active_process_list();
    } while (temp != curr);
    
    processInfo **info = allocMemory(globalMemoryManager, (count + 1) * sizeof(processInfo *));
    if (!info) return 0;
    
    int i = 0;
    do {
        PCB *pcb = curr->pcb;
        info[i] = allocMemory(globalMemoryManager, sizeof(processInfo));
        if (info[i]) {
            info[i]->name = strdup_kernel(pcb->name);
            info[i]->pid = pcb->pid;
            info[i]->parent = pcb->parent_pid;
            info[i]->rsp = (uint64_t *)pcb->stack_pointer;
            info[i]->rbp = (uint64_t *)((uint64_t)pcb->stack_pointer + 8);
            info[i]->priority = pcb->priority;
            info[i]->foreground = pcb->foreground;
            info[i]->status = pcb->state;
            info[i]->exitCode = 0; 
        }
        i++;
        curr = curr->next ? curr->next : get_active_process_list();
    } while (curr != get_active_process_list() && i < count);
    
    info[i] = NULL; 
    return (uint64_t)info;
}

static memoryData mem_info_buffer;

static uint64_t sys_mem_data() {
    getMemoryData(&mem_info_buffer);
    return (uint64_t)&mem_info_buffer;
}

static uint64_t sys_get_io_flags() {
    PCB *current = get_current_process();
    if (!current) {
        return 0;
    }
    return (current->stdin_pipe >= 0 ? 1 : 0) |
           (current->stdout_pipe >= 0 ? 2 : 0);
}
