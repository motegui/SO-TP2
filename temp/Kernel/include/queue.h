#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <mm_manager.h>

typedef struct QueueNode {
    int pid;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue {
    QueueNode *front;
    QueueNode *rear;
} Queue;

void queue_init(Queue *q);
bool queue_is_empty(Queue *q);
void queue_enqueue(Queue *q, int pid);
int queue_dequeue(Queue *q);
void queue_clear(Queue *q);

#endif