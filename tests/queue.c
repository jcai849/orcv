#include <assert.h>
#include "../src/queue.h"

int test_unused(void)
{
    Queue *empty_queue;

    empty_queue = make_queue();
    assert(queue_empty(empty_queue));
    rm_queue(empty_queue);
    return 0;
}

int test_symmetric_use(void)
{
    Queue *symmetric_queue;
    int item = 1;
    int *popped;

    symmetric_queue = make_queue();
    push_queue(symmetric_queue, &item);
    queue_empty(symmetric_queue);
    popped = pop_queue(symmetric_queue);
    assert(*popped == item);
    rm_queue(symmetric_queue);
    return 0;
}

int test_excess_push()
{
    Queue *pusher_queue;
    int items[] = {1, 2};
    int *popped;

    pusher_queue = make_queue();
    push_queue(pusher_queue, &items[0]);
    push_queue(pusher_queue, &items[1]);
    assert(!queue_empty(pusher_queue));
    popped = pop_queue(pusher_queue);
    assert(*popped = items[0]);
    rm_queue(pusher_queue);
    return 0;
}

int test_excess_pop()
{
    Queue *popper_queue;
    int item = 1;
    int *popped[2];

    popper_queue = make_queue();
    push_queue(popper_queue, &item);
    assert(!queue_empty(popper_queue));
    popped[0] = pop_queue(popper_queue);
    assert(*popped[0] == item);
    popped[1] = pop_queue(popper_queue);
    assert(popped[1] == NULL);
    rm_queue(popper_queue);
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
