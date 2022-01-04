#include "inputs.h"
#include "comms.h"

Message *multiplex(Inputs *inputs) {
    int i;
    Message *msg;

    if (poll(inputs->fds, POLL_ARRAY_SIZE, INFTIM) == -1) {
        perror(NULL);
        abort();
    }
    for (i = 0; i < POLL_ARRAY_SIZE; i++) {
        if (inputs->fds[i].revents & (POLLERR|POLLNVAL)) {
            perror(NULL);
            abort();
        }
        if (inputs->fds[i].revents & (POLLIN|POLLHUP)) {
            if (is_queue(inputs, inputs->fds[i].fd)) {
                msg = pop_event_queue(find_queue(inputs, inputs->fds[i].fd));
            } else {
                msg = receive(inputs->fds[i].fd);
                remove_fd(inputs, inputs->fds[i].fd);
            }
        }
    }
    return msg;
}
