#include <stdio.h>
#include "thread-safe-queue.h"

int tsqueue_init(TSQueue *queue)
{
  queue_init(&queue->queue);
  pthread_mutex_init(&queue->mutex, NULL);
  pthread_cond_init(&queue->non_empty, NULL);
  return 0;
}

void *tsqueue_dequeue(struct TSQueue *queue)
{
    void *pdata;

    if (pthread_mutex_lock(&queue->mutex) != 0) {
        perror(NULL);
    };
    while (queue_empty(queue->queue)) {
        if (pthread_cond_wait(&queue->non_empty, &queue->mutex) != 0) {
            perror(NULL);
        };
    }
    pdata = queue_dequeue(&queue->queue);
    if (pthread_mutex_unlock(&queue->mutex) != 0) {
        perror(NULL);
    };
    return pdata;
}

int tsqueue_enqueue(struct TSQueue *queue, void *pdata)
{
    int status;

    if (pthread_mutex_lock(&queue->mutex) != 0) {
        perror(NULL);
    };
    status = queue_enqueue(&queue->queue, pdata);
    if (pthread_cond_signal(&queue->non_empty) != 0) {
        perror(NULL);
    };
    if (pthread_mutex_unlock(&queue->mutex) != 0) {
        perror(NULL);
    };
    return status;
}

int tsqueue_destroy(struct TSQueue *queue)
{
    int status;

    if (pthread_cond_destroy(&queue->non_empty) != 0) {
        perror("couldn't destroy condition var");
    }
    if (pthread_mutex_destroy(&queue->mutex) != 0) {
        perror("couldn't destroy mutex");
    }
    status  = queue_destroy(&queue->queue);
    return status;
}
