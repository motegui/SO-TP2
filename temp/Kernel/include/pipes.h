#ifndef PIPES_H
#define PIPES_H

#include <stdint.h>
#include "sync.h"
#include "lib.h"
#include "print_utils.h"

#define PIPE_BUFFER_SIZE 4096
#define PIPE_NAME_LEN 32
#define MAX_PIPES 32


typedef struct {
    char name[PIPE_NAME_LEN];
    char buffer[PIPE_BUFFER_SIZE];

    int read_index;
    int write_index;
    int size;

    int open;
    uint8_t eof;

    Semaphore *filled_slots;
    Semaphore *empty_slots;
} Pipe;

void init_pipes();
int pipe_open(const char *name);
int pipe_write(int id, const char *src, unsigned int count);
int pipe_read(int id, char *dest, unsigned int count);
void pipe_close(int id);

#endif