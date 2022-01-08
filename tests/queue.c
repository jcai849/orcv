#include <assert.h>
#include <stddef.h>
#include "../src/queue.h"

int test_unused(void)
{
    Queue empty_queue;

    queue_init(&empty_queue);
    assert(queue_empty(empty_queue));
    queue_destroy(&empty_queue);
    return 0;
}

int test_symmetric_use(void)
{
    Queue symmetric_queue;
    int items[] = { 1, 2, 3};
    int *popped[3];

    queue_init(&symmetric_queue);
    queue_enqueue(&symmetric_queue, &items[0]);
    assert(!queue_empty(symmetric_queue));
    queue_enqueue(&symmetric_queue, &items[1]);
    assert(!queue_empty(symmetric_queue));
    popped[0] = queue_dequeue(&symmetric_queue);
    assert(*popped[0] == items[0]);
    popped[1] = queue_dequeue(&symmetric_queue);
    assert(*popped[1] == items[1]);
    assert(queue_empty(symmetric_queue));
    queue_enqueue(&symmetric_queue, &items[2]);
    popped[2] = queue_dequeue(&symmetric_queue);
    assert(*popped[2] == items[2]);
    queue_destroy(&symmetric_queue);
    return 0;
}

int test_excess_push()
{
    Queue pusher_queue;
    int items[] = {1, 2};
    int *popped;

    queue_init(&pusher_queue);
    queue_enqueue(&pusher_queue, &items[0]);
    queue_enqueue(&pusher_queue, &items[1]);
    assert(!queue_empty(pusher_queue));
    popped = queue_dequeue(&pusher_queue);
    assert(*popped = items[0]);
    queue_destroy(&pusher_queue);
    return 0;
}

int test_excess_pop()
{
    Queue popper_queue;
    int item = 1;
    int *popped[3];

    queue_init(&popper_queue);
    popped[0] = queue_dequeue(&popper_queue);
    assert(popped[0] == NULL);
    assert(queue_empty(popper_queue));
    queue_enqueue(&popper_queue, &item);
    assert(!queue_empty(popper_queue));
    popped[1] = queue_dequeue(&popper_queue);
    assert(*popped[1] == item);
    popped[2] = queue_dequeue(&popper_queue);
    assert(popped[2] == NULL);
    assert(queue_empty(popper_queue));
    queue_destroy(&popper_queue);
    return 0;
}

int main(void)
{
    test_unused();
    test_symmetric_use();
    test_excess_push();
    test_excess_pop();
    return 0;
}
