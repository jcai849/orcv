#include <stdio.h>
#include "thread-safe-queue.h"

int tsqueue_init(TSQueue *queue)
{
  queue_init(&queue->queue);
  if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
      perror(NULL);
      return -1;
  }
  if (pthread_cond_init(&queue->non_empty, NULL) != 0) {
      perror(NULL);
      return -1;
  }
  return 0;
}

void *tsqueue_dequeue(struct TSQueue *queue)
{
    void *pdata;

    if (pthread_mutex_lock(&queue->mutex) != 0) {
        perror(NULL);
        return NULL;
    }
    while (queue_empty(queue->queue)) {
        if (pthread_cond_wait(&queue->non_empty, &queue->mutex) != 0) {
            perror(NULL);
            return NULL;
        }
    }
    pdata = queue_dequeue(&queue->queue);
    if (pthread_mutex_unlock(&queue->mutex) != 0) {
        perror(NULL);
        return NULL;
    }
    return pdata;
}

int tsqueue_enqueue(struct TSQueue *queue, void *pdata)
{
    int status;

    if (pthread_mutex_lock(&queue->mutex) != 0) {
        perror(NULL);
        return -1;
    }
    status = queue_enqueue(&queue->queue, pdata);
    if (pthread_cond_signal(&queue->non_empty) != 0) {
        perror(NULL);
        return -1;
    }
    if (pthread_mutex_unlock(&queue->mutex) != 0) {
        perror(NULL);
        return -1;
    }
    return status;
}

int tsqueue_destroy(struct TSQueue *queue)
{
    if (pthread_cond_destroy(&queue->non_empty) != 0) {
        perror("couldn't destroy condition var");
        return -1;
    }
    if (pthread_mutex_destroy(&queue->mutex) != 0) {
        perror("couldn't destroy mutex");
        return -1;
    }
    return queue_destroy(&queue->queue);
}
