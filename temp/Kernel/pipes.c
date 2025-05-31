#include "pipes.h"

static Pipe pipe_table[MAX_PIPES];

void init_pipes(){
    for (int i = 0; i < MAX_PIPES; i++) {
        pipe_table[i].open = 0;
        pipe_table[i].eof = 0;
        pipe_table[i].read_index = 0;
        pipe_table[i].write_index = 0;
        pipe_table[i].size = 0;
    }
}

int pipe_open(const char *name){
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipe_table[i].open && strcmp(pipe_table[i].name, name) == 0) {
            return i;
        }
    }

    for (int i = 0; i < MAX_PIPES; i++) {
        if (!pipe_table[i].open) {
            strncpy(pipe_table[i].name, name, PIPE_NAME_LEN);

            pipe_table[i].read_index = 0;
            pipe_table[i].write_index = 0;
            pipe_table[i].size = 0;
            pipe_table[i].eof = 0;
            pipe_table[i].open = 1;

            char filled_name[PIPE_NAME_LEN + 8];
            char empty_name[PIPE_NAME_LEN + 8];
            snprintf(filled_name, sizeof(filled_name), "%s_filled", name);
            snprintf(empty_name, sizeof(empty_name), "%s_empty", name);

            pipe_table[i].filled_slots = semCreate(filled_name, 0);
            pipe_table[i].empty_slots = semCreate(empty_name, PIPE_BUFFER_SIZE);

            return i;
        }
    }

    return -1;
}

int pipe_write(int id, const char *src, unsigned int count){
    if (id < 0 || id >= MAX_PIPES || !pipe_table[id].open || pipe_table[id].eof) {
        return -1;
    }

    Pipe *pipe = &pipe_table[id];

    for (int i = 0; i < count; i++) {
        semWait(pipe->empty_slots);

        pipe->buffer[pipe->write_index] = src[i];
        pipe->write_index = (pipe->write_index + 1) % PIPE_BUFFER_SIZE;
        pipe->size++;

        semPost(pipe->filled_slots);
    }

    return count;
}

int pipe_read(int id, char *dest, unsigned int count) {
    if (id < 0 || id >= MAX_PIPES || !pipe_table[id].open) {
        return -1;
    }

    Pipe *pipe = &pipe_table[id];

    for (unsigned int i = 0; i < count; i++) {
        semWait(pipe->filled_slots);

        if (pipe->size == 0 && pipe->eof) {
            return i;
        }

        dest[i] = pipe->buffer[pipe->read_index];
        pipe->read_index = (pipe->read_index + 1) % PIPE_BUFFER_SIZE;
        pipe->size--;

        semPost(pipe->empty_slots);
    }

    return count;
}

void pipe_close(int id) {
    if (id < 0 || id >= MAX_PIPES || !pipe_table[id].open) {
        return;
    }

    Pipe *pipe = &pipe_table[id];

    semClose(pipe->filled_slots);
    semClose(pipe->empty_slots);

    pipe->open = 0;

    pipe->read_index = 0;
    pipe->write_index = 0;
    pipe->size = 0;
    pipe->eof = 0;
    pipe->name[0] = '\0';
}