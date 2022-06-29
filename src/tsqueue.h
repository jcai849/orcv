#include "queue.h"
#include <pthread.h>

typedef struct TSQueue {
    Queue queue;
    pthread_mutex_t mutex;
    pthread_cond_t non_empty;
} TSQueue;

int tsqueue_init(TSQueue *queue);
void *tsqueue_dequeue(TSQueue *queue);
int tsqueue_enqueue(TSQueue *queue, void *pdata);
int tsqueue_destroy(TSQueue *queue);
