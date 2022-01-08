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

    inputs_insert_fd(&inputs, fds[0]);
    inputs_remove_fd(&inputs, fds[0]);

    inputs_insert_fd(&inputs, fds[1]);
    inputs_insert_fd(&inputs, fds[2]);
    inputs_insert_fd(&inputs, fds[3]);
    inputs_insert_fd(&inputs, fds[4]);
    inputs_remove_fd(&inputs, fds[1]);

    inputs_destroy(&inputs);
    return 0;
}

int main(void)
{
    test_unused();
    test_symmetric_fd();
    test_excess_fd();
    // test_excess_remove_fd();
    // test_symmetric_queue();
    // test_excess_queue();
    // test_excess_remove_queue();
    // test_mixed(); /* big; testing removal and addition */
    // test_is_queue();
    // test_queue_retrieval();
    // return 0;
}
