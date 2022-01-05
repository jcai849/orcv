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
    int item;
    int *popped;

    symmetric_queue = make_queue();
    item = 1;
    push_queue(symmetric_queue, &item);
    queue_empty(symmetric_queue);
    popped = pop_queue(symmetric_queue);
    assert(*popped == 1);
    rm_queue(symmetric_queue);
    return 0;
}

int test_asymmetric_use()
{
    Queue *asymmetric_queue;
    int items[] = { 1, 2, 3, 4};
    int *popped[2];

    asymmetric_queue = make_queue();
    push_queue(asymmetric_queue, &items[0]);
    push_queue(asymmetric_queue, &items[1]);
    assert(!queue_empty(asymmetric_queue));
    popped[0] = pop_queue(asymmetric_queue);
    assert(*popped[0] == items[0]);
    popped[1] = pop_queue(asymmetric_queue);
    assert(*popped[1] == items[1]);
    push_queue(asymmetric_queue, &items[2]);
    push_queue(asymmetric_queue, &items[3]);
    rm_queue(asymmetric_queue);
    return 0;
}

int main(void)
{
    test_unused();
    test_symmetric_use();
    test_asymmetric_use();
    return 0;
}
