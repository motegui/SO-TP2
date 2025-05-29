#include "queue.h"
#include <stdlib.h>

void queue_init(Queue *q) {
    q->front = NULL;
    q->rear = NULL;
}

bool queue_is_empty(Queue *q) {
    return q->front == NULL;
}

void queue_enqueue(Queue *q, int pid) {
    QueueNode *node = malloc(sizeof(QueueNode));
    node->pid = pid;
    node->next = NULL;

    if (q->rear)
        q->rear->next = node;
    else
        q->front = node;

    q->rear = node;
}

int queue_dequeue(Queue *q) {
    if (queue_is_empty(q))
        return -1;

    QueueNode *node = q->front;
    int pid = node->pid;
    q->front = node->next;

    if (!q->front)
        q->rear = NULL;

    free(node);
    return pid;
}

void queue_clear(Queue *q) {
    while (!queue_is_empty(q)) {
        queue_dequeue(q);
    }
}