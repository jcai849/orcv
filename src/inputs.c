#include <poll.h>
#include <stdio.h>
#include <stdlib.h>

#include "inputs.h"

int inputs_init(struct Inputs *inputs)
{
    int i;

    for (i = 0; i < POLL_ARRAY_SIZE; i++) {
        inputs->fds[i].fd = -1;
        inputs->fds[i].events = POLLIN;
    }

    inputs->queues = NULL;

    return 0;
}

int inputs_insert_fd(struct Inputs *inputs, int fd)
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

int inputs_insert_queue(struct Inputs *inputs, struct EventQueue *queue)
{
    EventQueueNode *node;

    inputs_insert_fd(inputs, queue->fd[0]);
    if ((node = malloc(sizeof(*node))) == NULL) {
        perror(NULL);
        abort();
    }
    node->queue = queue;
    node->next = inputs->queues;
    inputs->queues = node;
    return 0;
}

int inputs_remove_fd(struct Inputs *inputs, int fd)
{
    int i = 0;
    while (inputs->fds[i].fd != fd && i < POLL_ARRAY_SIZE) {i++;}
    if (i == POLL_ARRAY_SIZE) {
        fprintf(stderr, "fd not found\n");
        return 1;
    }
    inputs->fds[i].fd = -1;
    return 0;
}

int inputs_remove_queue_fd(struct Inputs *inputs, int fd)
{
    struct EventQueueNode *curr, *next;

    inputs_remove_fd(inputs, fd);
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

int inputs_fd_is_queue(struct Inputs *inputs, int fd)
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

EventQueue *inputs_retrieve_queue(struct Inputs *inputs, int fd)
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

int delete_queue_nodes(struct EventQueueNode *queues) // N.B. Doesn't delete the actual queues.
{
    struct EventQueueNode *tmp;

    tmp = queues;
    while (queues != NULL) {
        tmp = queues->next;
        free(queues);
        queues = tmp;
    }
    return 0;
}

int inputs_destroy(struct Inputs *inputs)
{
    delete_queue_nodes(inputs->queues);
    return 0;
}
