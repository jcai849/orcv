#include "event-queue.h"
#include <unistd.h>
#include <stdio.h>

int event_queue_init(struct EventQueue *queue)
{
    tsqueue_init(&queue->queue);
    if (pipe(queue->fd) == -1) {
        perror(NULL);
    };

    return 0;
}

void *event_queue_dequeue(struct EventQueue *queue)
{
    void *pdata;
    unsigned char signal[1] = {0};

    pdata = tsqueue_dequeue(&queue->queue);
    if (read(queue->fd[1], signal, sizeof(char)) == -1) {
        perror(NULL);
    };
    return pdata;
}

int event_queue_enqueue(struct EventQueue *queue, void *pdata)
{
    unsigned char signal[1] = {0};
    int status;

    status = tsqueue_enqueue(&queue->queue, pdata);
    if (write(queue->fd[0], signal, sizeof(char)) == -1) {
        perror(NULL);
    }
    return 0;
}

int event_queue_destroy(struct EventQueue *queue)
{
    int status;

    status = tsqueue_destroy(&queue->queue);
    if (close(queue->fd[0]) == -1 || close(queue->fd[1]) == -1) {
        perror(NULL);
    }
    return 0;
}
