#include <poll.h>
#include "event-queue.h"

#define POLL_ARRAY_SIZE 100

typedef struct EventQueueNode {
    struct EventQueue* queue;
    struct EventQueueNode *next;
} EventQueueNode;

typedef struct Inputs {
    struct pollfd fds[POLL_ARRAY_SIZE];
    EventQueueNode *queues;
} Inputs;

int inputs_init(Inputs *);
int inputs_insert_fd(Inputs *, int);
int inputs_insert_queue(Inputs *, EventQueue *);
int inputs_fd_is_queue(Inputs*, int);
EventQueue *inputs_retrieve_queue(Inputs *, int);
int inputs_remove_fd(Inputs *, int);
int inputs_remove_queue_fd(Inputs *, int);
int inputs_destroy(Inputs *);
