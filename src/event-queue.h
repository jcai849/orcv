#include "thread-safe-queue.h"

typedef struct EventQueue {
    TSQueue queue;
    int fd[2]; /* read, write */
} EventQueue;

int event_queue_init(EventQueue *);
void *event_queue_dequeue(EventQueue *);
int event_queue_enqueue(EventQueue *, void *);
int event_queue_destroy(EventQueue *);
