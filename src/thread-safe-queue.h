#include "queue.h"
#include <pthread.h>

typedef struct TSQueue {
    Queue *queue;
    pthread_mutex_t *mutex;
    pthread_cond_t *non_empty;
} TSQueue;

TSQueue *make_ts_queue(void);
void *pop_ts_queue(TSQueue *);
int push_ts_queue(TSQueue *, void *);
int rm_ts_queue(TSQueue *);
