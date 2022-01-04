#include "event-queue.h"
#include <unistd.h>

struct EventQueue *make_event_queue(void)
{
    struct EventQueue *queue;

    if ((queue = malloc(sizeof(struct EventQueue))) == NULL) {
        perror(NULL);
        abort();
    }

    queue->queue = make_ts_queue();
    if (pipe(queue->fd) == -1) {
        perror(NULL);
        abort();
    };

    return queue;
}

void *pop_event_queue(struct EventQueue *queue)
{
    void *pdata;
    unsigned char buf[1] = {0};

    pdata = pop_ts_queue(queue->queue);
    if (read(queue->fd[1], buf, sizeof(char)) == -1) {
        perror(NULL);
        abort();
    };
    return pdata;
}

int push_event_queue(struct EventQueue *queue, void *pdata)
{
    unsigned char buf[1] = {0};
    int status;

    status = push_ts_queue(queue->queue, pdata);
    if (write(queue->fd[0], buf, sizeof(char)) == -1) {
        perror(NULL);
        abort();
    }
    return 0;
}

int rm_event_queue(struct EventQueue *queue)
{
    int status;

    status = rm_ts_queue(queue->queue);
    if (close(queue->fd[0]) == -1 || close(queue->fd[1]) == -1) {
        perror(NULL);
        abort();
    }
    free(queue);
    return 0;
}
