#include "thread-safe-queue.h"

typedef struct EventQueue {
    TSQueue *queue;
    int fd[2]; /* read, write */
} EventQueue;

EventQueue *make_event_queue(void);
void *pop_event_queue(EventQueue *);
int push_event_queue(EventQueue *, void *);
int rm_event_queue(EventQueue *);
