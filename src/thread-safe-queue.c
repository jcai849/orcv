#include "thread-safe-queue.h"

struct TSQueue *make_ts_queue(void)
{
  struct TSQueue *queue;

  if ((queue = malloc(sizeof(struct TSQueue))) == NULL) {
    perror(NULL);
    abort();
  }
  queue->queue = make_queue();

  if ((queue->mutex = malloc(sizeof(pthread_mutex_t))) == NULL) {
    perror(NULL);
    abort();
  }
  if (pthread_mutex_init(queue->mutex, NULL) != 0) {
    perror(NULL);
    abort();
  }
  if ((queue->non_empty = malloc(sizeof(pthread_cond_t))) == NULL) {
    perror(NULL);
    abort();
  }
  if (pthread_cond_init(queue->non_empty, NULL) != 0) {
    perror(NULL);
    abort();
  }

  return queue;
}

void *pop_ts_queue(struct TSQueue *queue)
{
    void *pdata;

    if (pthread_mutex_lock(queue->mutex) != 0) {
        perror(NULL);
        abort();
    };
    while (queue_empty(queue->queue)) {
        if (pthread_cond_wait(queue->non_empty, queue->mutex) != 0) {
            perror(NULL);
            abort();
        };
    }
    pdata = pop_queue(queue->queue);
    if (pthread_mutex_unlock(queue->mutex) != 0) {
        perror(NULL);
        abort();
    };
    return pdata;
}

int push_ts_queue(struct TSQueue *queue, void *pdata)
{
    int status;

    if (pthread_mutex_lock(queue->mutex) != 0) {
        perror(NULL);
        abort();
    };
    status = push_queue(queue->queue, pdata);
    if (pthread_cond_signal(queue->non_empty) != 0) {
        perror(NULL);
        abort();
    };
    if (pthread_mutex_unlock(queue->mutex) != 0) {
        perror(NULL);
        abort();
    };
    return status;
}

int rm_ts_queue(struct TSQueue *queue)
{
    int status;

    status  = rm_queue(queue->queue);
    free(queue);
    return status;
}
