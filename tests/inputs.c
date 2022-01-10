#include <assert.h>
#include "../src/inputs.h"

int test_unused(void)
{
    Inputs inputs;

    inputs_init(&inputs);
    inputs_destroy(&inputs);
    return 0;
}

int test_symmetric_fd(void)
{
    Inputs inputs;
    int fds[] = { 0, 1, 2 };

    inputs_init(&inputs);

    inputs_insert_fd(&inputs, fds[0]);
    inputs_remove_fd(&inputs, fds[0]);

    inputs_insert_fd(&inputs, fds[1]);
    inputs_insert_fd(&inputs, fds[2]);
    inputs_remove_fd(&inputs, fds[2]);
    inputs_remove_fd(&inputs, fds[1]);

    inputs_destroy(&inputs);
    return 0;
}

int test_excess_fd(void)
{
    Inputs inputs;
    int fds[] = { 0, 1, 2, 3, 4};

    inputs_init(&inputs);

    assert(!inputs_insert_fd(&inputs, fds[0]));
    assert(!inputs_remove_fd(&inputs, fds[0]));

    assert(!inputs_insert_fd(&inputs, fds[1]));
    assert(!inputs_insert_fd(&inputs, fds[2]));
    assert(!inputs_insert_fd(&inputs, fds[3]));
    assert(!inputs_insert_fd(&inputs, fds[4]));
    assert(!inputs_remove_fd(&inputs, fds[1]));

    inputs_destroy(&inputs);
    return 0;
}

int test_excess_remove_fd(void)
{
    Inputs inputs;
    int fds[] = { 0, 1 };

    inputs_init(&inputs);

    assert(!inputs_insert_fd(&inputs, fds[0]));
    assert(!inputs_remove_fd(&inputs, fds[0]));
    assert(inputs_remove_fd(&inputs, fds[0]));

    assert(!inputs_insert_fd(&inputs, fds[1]));
    assert(!inputs_remove_fd(&inputs, fds[1]));
    assert(inputs_remove_fd(&inputs, fds[1]));
    assert(inputs_remove_fd(&inputs, fds[1]));

    inputs_destroy(&inputs);
    return 0;
}

int test_symmetric_queue(void)
{
    Inputs inputs;
    EventQueue queues[3];
    int i;

    inputs_init(&inputs);
    for (i = 0; i < 3; i++) {
        event_queue_init(&queues[i]);
    }

    inputs_insert_queue(&inputs, &queues[0]);
    inputs_remove_queue_fd(&inputs, queues[0].fd[0]);

    inputs_insert_queue(&inputs, &queues[1]);
    inputs_insert_queue(&inputs, &queues[2]);
    inputs_remove_queue_fd(&inputs, queues[2].fd[0]);
    inputs_remove_queue_fd(&inputs, queues[1].fd[0]);

    inputs_destroy(&inputs);
    for (i = 0; i < 3; i++) {
        event_queue_destroy(&queues[i]);
    }

    return 0;
}

int test_excess_queue(void)
{
    Inputs inputs;
    EventQueue queues[5];
    int i;

    inputs_init(&inputs);
    for (i = 0; i < 5; i++) {
        event_queue_init(&queues[i]);
    }

    assert(!inputs_insert_queue(&inputs, &queues[0]));
    assert(!inputs_remove_queue_fd(&inputs, queues[0].fd[0]));

    assert(!inputs_insert_queue(&inputs, &queues[1]));
    assert(!inputs_insert_queue(&inputs, &queues[2]));
    assert(!inputs_insert_queue(&inputs, &queues[3]));
    assert(!inputs_insert_queue(&inputs, &queues[4]));
    assert(!inputs_remove_queue_fd(&inputs, queues[1].fd[0]));

    inputs_destroy(&inputs);
    for (i = 0; i < 5; i++) {
        event_queue_destroy(&queues[i]);
    }
    return 0;
}

int test_excess_remove_queue(void)
{
    Inputs inputs;
    EventQueue queues[2];
    int i;

    inputs_init(&inputs);
    for (i = 0; i < 2; i++) {
        event_queue_init(&queues[i]);
    }

    assert(!inputs_insert_queue(&inputs, &queues[0]));
    assert(!inputs_remove_queue_fd(&inputs, queues[0].fd[0]));
    assert(inputs_remove_queue_fd(&inputs, queues[0].fd[0]));

    assert(!inputs_insert_queue(&inputs, &queues[1]));
    assert(!inputs_remove_queue_fd(&inputs, queues[1].fd[0]));
    assert(inputs_remove_queue_fd(&inputs, queues[1].fd[0]));
    assert(inputs_remove_queue_fd(&inputs, queues[1].fd[0]));

    inputs_destroy(&inputs);
    for (i = 0; i < 2; i++) {
        event_queue_destroy(&queues[i]);
    }
    return 0;
}

int test_mixed(void)
{
    Inputs inputs;
    EventQueue queues[3];
    int fds[] = { 0, 1, 2 };
    int i;

    inputs_init(&inputs);
    for (i = 0; i < 3; i++) {
        event_queue_init(&queues[i]);
    }

    assert(!inputs_insert_fd(&inputs, fds[0]));
    assert(!inputs_insert_queue(&inputs, &queues[0]));
    assert(!inputs_insert_fd(&inputs, fds[1]));
    assert(!inputs_insert_queue(&inputs, &queues[1]));

    assert(!inputs_remove_fd(&inputs, fds[0]));
    assert(!inputs_remove_queue_fd(&inputs, queues[0].fd[0]));
    assert(!inputs_remove_fd(&inputs, fds[1]));
    assert(!inputs_remove_queue_fd(&inputs, queues[1].fd[0]));

    assert(!inputs_insert_fd(&inputs, fds[2]));
    assert(!inputs_insert_queue(&inputs, &queues[2]));

    inputs_destroy(&inputs);
    for (i = 0; i < 3; i++) {
        event_queue_destroy(&queues[i]);
    }
    return 0;
}

int test_is_queue(void)
{
    Inputs inputs;
    EventQueue queues[3];
    int fds[] = { 0, 1, 2 };
    int i;

    inputs_init(&inputs);
    for (i = 0; i < 3; i++) {
        event_queue_init(&queues[i]);
    }
    assert(!inputs_insert_fd(&inputs, fds[0]));
    assert(!inputs_insert_queue(&inputs, &queues[0]));
    assert(!inputs_insert_fd(&inputs, fds[1]));
    assert(!inputs_insert_queue(&inputs, &queues[1]));

    assert(inputs_fd_is_queue(&inputs, queues[0].fd[0]));
    assert(!inputs_fd_is_queue(&inputs, fds[0]));
    assert(inputs_fd_is_queue(&inputs, queues[1].fd[0]));
    assert(!inputs_fd_is_queue(&inputs, fds[1]));

    inputs_insert_queue(&inputs, &queues[2]);
    inputs_insert_fd(&inputs, fds[2]);

    inputs_remove_queue_fd(&inputs, queues[0].fd[0]);
    assert(!inputs_fd_is_queue(&inputs, queues[0].fd[0]));
    inputs_remove_fd(&inputs, fds[0]);
    assert(!inputs_fd_is_queue(&inputs, fds[0]));
    assert(inputs_fd_is_queue(&inputs, queues[1].fd[0]));
    assert(!inputs_fd_is_queue(&inputs, fds[1]));

    inputs_destroy(&inputs);
    for (i = 0; i < 3; i++) {
        event_queue_destroy(&queues[i]);
    }
    return 0;
}

int test_queue_retrieval(void)
{
    Inputs inputs;
    EventQueue queues[3];
    int fds[] = { 0, 1 };
    int i;

    inputs_init(&inputs);
    for (i = 0; i < 3; i++) {
        event_queue_init(&queues[i]);
    }

    assert(inputs_retrieve_queue(&inputs, queues[0].fd[0]) == NULL);
    inputs_insert_queue(&inputs, &queues[0]);
    assert(inputs_retrieve_queue(&inputs, queues[0].fd[0]) == &queues[0]);
    inputs_remove_queue_fd(&inputs, queues[0].fd[0]);
    assert(inputs_retrieve_queue(&inputs, queues[0].fd[0]) == NULL);
    inputs_insert_queue(&inputs, &queues[0]);
    inputs_insert_queue(&inputs, &queues[1]);
    assert(inputs_retrieve_queue(&inputs, queues[1].fd[0]) == &queues[1]);
    inputs_remove_queue_fd(&inputs, queues[1].fd[0]);
    assert(inputs_retrieve_queue(&inputs, queues[1].fd[0]) == NULL);
    inputs_remove_queue_fd(&inputs, queues[0].fd[0]);

    assert(inputs_retrieve_queue(&inputs, fds[0]) == NULL);
    inputs_insert_fd(&inputs, fds[0]);
    assert(inputs_retrieve_queue(&inputs, fds[0]) == NULL);
    inputs_insert_fd(&inputs, fds[1]);
    assert(inputs_retrieve_queue(&inputs, fds[1]) == NULL);
    inputs_insert_queue(&inputs, &queues[2]);
    assert(inputs_retrieve_queue(&inputs, queues[2].fd[0]) == &queues[2]);


    inputs_destroy(&inputs);
    for (i = 0; i < 3; i++) {
        event_queue_destroy(&queues[i]);
    }
    return 0;
}

int main(void)
{
    test_unused();
    test_symmetric_fd();
    test_excess_fd();
    test_excess_remove_fd();
    test_symmetric_queue();
    test_excess_queue();
    test_excess_remove_queue();
    test_mixed();
    test_is_queue();
    test_queue_retrieval();
    return 0;
}
