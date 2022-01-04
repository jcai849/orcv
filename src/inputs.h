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

Inputs *make_inputs(void);
int add_fd(Inputs *, int);
int add_queue(Inputs *, EventQueue *);
int is_queue(Inputs*, int);
EventQueue *find_queue(Inputs *, int);
int remove_fd(Inputs *, int);
int remove_queue_fd(Inputs *, int);
int delete_inputs(Inputs *);
