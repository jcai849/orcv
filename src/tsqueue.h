#include "queue.h"
#include <pthread.h>

typedef struct TSQueue {
    Queue queue;
    pthread_mutex_t mutex;
    pthread_cond_t non_empty;
} TSQueue;

int tsqueue_init(TSQueue *);
void *tsqueue_dequeue(TSQueue *);
int tsqueue_enqueue(TSQueue *, void *);
int tsqueue_destroy(TSQueue *);
