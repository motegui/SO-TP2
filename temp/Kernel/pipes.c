#include "pipes.h"
#include "lib.h"

static Pipe pipe_table[MAX_PIPES];

static void wake_waiters(sem_t sem) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        sem_post(sem);
    }
}

static void reset_pipe(Pipe *pipe) {
    pipe->name[0] = '\0';
    pipe->read_index = 0;
    pipe->write_index = 0;
    pipe->size = 0;
    pipe->open = 0;
    pipe->readers = 0;
    pipe->writers = 0;
    pipe->reader_end_closed = 0;
    pipe->writer_end_closed = 0;
    pipe->lock = 0;
    pipe->filled_slots = NULL;
    pipe->empty_slots = NULL;
}

static void destroy_if_unused(Pipe *pipe) {
    if (pipe->open && pipe->readers == 0 && pipe->writers == 0 &&
        pipe->reader_end_closed && pipe->writer_end_closed) {
        sem_close(pipe->filled_slots);
        sem_close(pipe->empty_slots);
        reset_pipe(pipe);
    }
}

void init_pipes(){
    for (int i = 0; i < MAX_PIPES; i++) {
        reset_pipe(&pipe_table[i]);
    }
}

int pipe_open(const char *name){
    if (name == NULL) {
        return -1;
    }

    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipe_table[i].open && lib_strcmp(pipe_table[i].name, name) == 0) {
            return i;
        }
    }

    for (int i = 0; i < MAX_PIPES; i++) {
        if (!pipe_table[i].open) {
            Pipe *pipe = &pipe_table[i];
            reset_pipe(pipe);
            lib_strncpy(pipe->name, name, PIPE_NAME_LEN - 1);
            pipe->name[PIPE_NAME_LEN - 1] = '\0';
            pipe->filled_slots = sem_create(0);
            pipe->empty_slots = sem_create(PIPE_BUFFER_SIZE);
            if (pipe->filled_slots == NULL || pipe->empty_slots == NULL) {
                if (pipe->filled_slots != NULL) {
                    sem_close(pipe->filled_slots);
                }
                if (pipe->empty_slots != NULL) {
                    sem_close(pipe->empty_slots);
                }
                reset_pipe(pipe);
                return -1;
            }
            pipe->open = 1;
            return i;
        }
    }

    return -1;
}

int pipe_attach_reader(int id) {
    if (id < 0 || id >= MAX_PIPES || !pipe_table[id].open) {
        return -1;
    }

    Pipe *pipe = &pipe_table[id];
    enter_region(&pipe->lock);
    if (pipe->reader_end_closed) {
        leave_region(&pipe->lock);
        return -1;
    }
    pipe->readers++;
    leave_region(&pipe->lock);
    return 0;
}

int pipe_attach_writer(int id) {
    if (id < 0 || id >= MAX_PIPES || !pipe_table[id].open) {
        return -1;
    }

    Pipe *pipe = &pipe_table[id];
    enter_region(&pipe->lock);
    if (pipe->writer_end_closed) {
        leave_region(&pipe->lock);
        return -1;
    }
    pipe->writers++;
    leave_region(&pipe->lock);
    return 0;
}

int pipe_write(int id, const char *src, unsigned int count){
    if (id < 0 || id >= MAX_PIPES || !pipe_table[id].open || src == NULL) {
        return -1;
    }

    Pipe *pipe = &pipe_table[id];
    unsigned int written = 0;

    while (written < count) {
        enter_region(&pipe->lock);
        if (pipe->reader_end_closed) {
            leave_region(&pipe->lock);
            return written > 0 ? (int)written : -1;
        }
        leave_region(&pipe->lock);

        if (sem_wait(pipe->empty_slots) < 0) {
            return written > 0 ? (int)written : -1;
        }

        enter_region(&pipe->lock);
        if (pipe->reader_end_closed || !pipe->open) {
            leave_region(&pipe->lock);
            sem_post(pipe->empty_slots);
            return written > 0 ? (int)written : -1;
        }

        pipe->buffer[pipe->write_index] = src[written++];
        pipe->write_index = (pipe->write_index + 1) % PIPE_BUFFER_SIZE;
        pipe->size++;
        leave_region(&pipe->lock);

        sem_post(pipe->filled_slots);
    }

    return (int)written;
}

int pipe_read(int id, char *dest, unsigned int count) {
    if (id < 0 || id >= MAX_PIPES || !pipe_table[id].open || dest == NULL) {
        return -1;
    }

    Pipe *pipe = &pipe_table[id];
    unsigned int read = 0;

    while (read < count) {
        enter_region(&pipe->lock);
        if (pipe->size == 0 && pipe->writer_end_closed) {
            leave_region(&pipe->lock);
            return (int)read;
        }
        leave_region(&pipe->lock);

        if (sem_wait(pipe->filled_slots) < 0) {
            return read > 0 ? (int)read : -1;
        }

        enter_region(&pipe->lock);
        if (pipe->size == 0) {
            int done = pipe->writer_end_closed || !pipe->open;
            leave_region(&pipe->lock);
            if (done) {
                return (int)read;
            }
            continue;
        }

        dest[read++] = pipe->buffer[pipe->read_index];
        pipe->read_index = (pipe->read_index + 1) % PIPE_BUFFER_SIZE;
        pipe->size--;
        leave_region(&pipe->lock);

        sem_post(pipe->empty_slots);
    }

    return (int)read;
}

void pipe_close_reader(int id) {
    if (id < 0 || id >= MAX_PIPES || !pipe_table[id].open) {
        return;
    }

    Pipe *pipe = &pipe_table[id];
    enter_region(&pipe->lock);
    if (pipe->readers > 0) {
        pipe->readers--;
    }
    if (pipe->readers == 0) {
        pipe->reader_end_closed = 1;
    }
    leave_region(&pipe->lock);

    wake_waiters(pipe->empty_slots);
    destroy_if_unused(pipe);
}

void pipe_close_writer(int id) {
    if (id < 0 || id >= MAX_PIPES || !pipe_table[id].open) {
        return;
    }

    Pipe *pipe = &pipe_table[id];
    enter_region(&pipe->lock);
    if (pipe->writers > 0) {
        pipe->writers--;
    }
    if (pipe->writers == 0) {
        pipe->writer_end_closed = 1;
    }
    leave_region(&pipe->lock);

    wake_waiters(pipe->filled_slots);
    destroy_if_unused(pipe);
}

void pipe_shutdown_write(int id) {
    pipe_close_writer(id);
}

void pipe_close(int id) {
    if (id < 0 || id >= MAX_PIPES || !pipe_table[id].open) {
        return;
    }

    Pipe *pipe = &pipe_table[id];
    enter_region(&pipe->lock);
    if (pipe->readers == 0) {
        pipe->reader_end_closed = 1;
    }
    if (pipe->writers == 0) {
        pipe->writer_end_closed = 1;
    }
    int no_refs = pipe->readers == 0 && pipe->writers == 0;
    leave_region(&pipe->lock);

    if (no_refs) {
        wake_waiters(pipe->filled_slots);
        wake_waiters(pipe->empty_slots);
        sem_close(pipe->filled_slots);
        sem_close(pipe->empty_slots);
        reset_pipe(pipe);
    }
}
