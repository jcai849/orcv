#include "inputs.h"

struct Inputs *make_inputs(void)
{
    struct Inputs *inputs;
    int i;

    if ((inputs = malloc(sizeof(*inputs))) == NULL) {
        perror(NULL);
        abort();
    }

    for (i = 0; i < POLL_ARRAY_SIZE; i++) {
        inputs->fds[i].fd = -1;
        inputs->fds[i].events = POLLIN;
    }

    inputs->queues = NULL;

    return inputs;
}

int add_fd(struct Inputs *inputs, int fd)
{
    int i = 0;
    while (inputs->fds[i].fd != -1 && i < POLL_ARRAY_SIZE) {i++;}
    if (i == POLL_ARRAY_SIZE) {
        fprintf(stderr, "Poll array size too small\n");
        abort();
    }
    inputs->fds[i].fd = fd;
    return 0;
}

int add_queue(struct Inputs *inputs, EventQueue *queue)
{
    EventQueueNode *node;

    add_fd(inputs, queue->fd[0]);
    if ((node = malloc(sizeof(*node))) == NULL) {
        perror(NULL);
        abort();
    }
    node->queue = queue;
    node->next = inputs->queues;
    inputs->queues = node;
    return 0;
}

int remove_fd(struct Inputs *inputs, int fd)
{
    int i = 0;
    while (inputs->fds[i].fd != fd && i < POLL_ARRAY_SIZE) {i++;}
    if (i == POLL_ARRAY_SIZE) {
        fprintf(stderr, "fd not found\n");
        abort();
    }
    inputs->fds[i].fd = -1;
    return 0;
}

int remove_queue_fd(struct Inputs *inputs, int fd)
{
    EventQueueNode *curr, *next;

    remove_fd(inputs, fd);
    curr = inputs->queues;
    if (curr  == NULL) return 1;
    if (curr->queue->fd[0] == fd) {
        inputs->queues = curr->next;
        free(curr);
        return 0;
    }
    next = inputs->queues->next;
    while (next != NULL) {
        if (next->queue->fd[0] == fd) {
            curr->next = next->next;
            free(next);
            return 0;
        }
    }
    return 1;
}

int is_queue(struct Inputs *inputs, int fd)
{
    struct EventQueueNode *curr;

    curr = inputs->queues;
    while (curr != NULL) {
        if (curr->queue->fd[0] == fd)
            return 1;
        curr = curr->next;
    }
    return 0;
}

EventQueue *find_queue(struct Inputs *inputs, int fd)
{
    struct EventQueueNode *curr;

    curr = inputs->queues;
    while (curr != NULL) {
        if (curr->queue->fd[0] == fd)
            return curr->queue;
        curr = curr->next;
    }
    fprintf(stderr, "Queue not found\n");
    abort();
}

int delete_queue_nodes(EventQueueNode *queues) // N.B. Doesn't delete the queues.
{
    EventQueueNode *tmp;

    tmp = queues;
    while (queues != NULL) {
        tmp = queues->next;
        free(queues);
        queues = tmp;
    }
    return 0;
}

int delete_inputs(struct Inputs *inputs)
{
    delete_queue_nodes(inputs->queues);
    free(inputs);
    return 0;
}
