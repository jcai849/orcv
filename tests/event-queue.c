#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include "../src/event-queue.h"

int test_unused(void)
{
    EventQueue unused_queue;

    event_queue_init(&unused_queue);
    event_queue_destroy(&unused_queue);
    return 0;
}

int test_symmetric(void)
{
    EventQueue symmetric_queue;
    int items[] = {1, 2, 3};
    int *popped[3];

    event_queue_init(&symmetric_queue);
    event_queue_enqueue(&symmetric_queue, &items[0]);
    event_queue_enqueue(&symmetric_queue, &items[1]);
    popped[0] = event_queue_dequeue(&symmetric_queue);
    assert(*popped[0] == items[0]);
    popped[1] = event_queue_dequeue(&symmetric_queue);
    assert(*popped[1] == items[1]);
    event_queue_enqueue(&symmetric_queue, &items[2]);
    popped[2] = event_queue_dequeue(&symmetric_queue);
    assert(*popped[2] == items[2]);
    event_queue_destroy(&symmetric_queue);
    return 0;
}

int test_excess_push(void)
{
    EventQueue pusher_queue;
    int items[] = { 1, 2, 3 };
    int *popped;

    event_queue_init(&pusher_queue);
    event_queue_enqueue(&pusher_queue, &items[0]);
    event_queue_enqueue(&pusher_queue, &items[1]);
    event_queue_enqueue(&pusher_queue, &items[2]);
    popped = event_queue_dequeue(&pusher_queue);
    assert(*popped == items[0]);
    event_queue_destroy(&pusher_queue);
    return 0;
}

struct pusher_arg {
    EventQueue *queue;
    int *item;
};

struct popper_arg {
    EventQueue *queue;
    int *popped;
};

void *pusher(void *arg)
{
    event_queue_enqueue(((struct pusher_arg *) arg)->queue, ((struct pusher_arg *) arg)->item);
    pthread_exit(NULL);
}

void *popper(void *arg)
{
    event_queue_dequeue(((struct pusher_arg *) arg)->queue);
    pthread_exit(NULL);
}

int test_push_first(void)
{
    EventQueue queue;
    struct pusher_arg pusharg[3];
    struct popper_arg poparg[3];
    pthread_t pushers[3], poppers[3];
    void *ret;
    int i;

    int items[] = { 1, 2, 3};
    int *popped[3];

    event_queue_init(&queue);

    for (i = 0; i < 3; i++) {
        pusharg[i].queue = &queue;
        pusharg[i].item = &items[i];
        pthread_create(&pushers[i], NULL, &pusher, &pusharg[i]);
    }
    for (i = 0; i < 3; i++) {
        pthread_join(pushers[i], &ret);
    }
    for (i = 0; i < 3; i++) {
        poparg[i].queue = &queue;
        poparg[i].popped = popped[i];
        pthread_create(&poppers[i], NULL, &popper, &poparg[i]);
    }
    for (i = 0; i < 3; i++) {
        pthread_join(poppers[i], &ret);
    }
    event_queue_destroy(&queue);
    return 0;
}

int test_pop_first(void)
{
    EventQueue queue;
    struct pusher_arg pusharg[3];
    struct popper_arg poparg[3];
    pthread_t pushers[3], poppers[3];
    void *ret;
    int i;

    int items[] = { 1, 2, 3};
    int *popped[3];

    event_queue_init(&queue);

    for (i = 0; i < 3; i++) {
        poparg[i].queue = &queue;
        poparg[i].popped = popped[i];
        pthread_create(&poppers[i], NULL, &popper, &poparg[i]);
    }
    sleep(1);
    for (i = 0; i < 3; i++) {
        pusharg[i].queue = &queue;
        pusharg[i].item = &items[i];
        pthread_create(&pushers[i], NULL, &pusher, &pusharg[i]);
    }
    for (i = 0; i < 3; i++) {
        pthread_join(pushers[i], &ret);
    }
    for (i = 0; i < 3; i++) {
        pthread_join(poppers[i], &ret);
    }
    event_queue_destroy(&queue);
    return 0;
}

int main(void)
{
    test_unused();
    test_symmetric();
    test_excess_push();
    test_push_first();
    test_pop_first();
    return 0;
}
