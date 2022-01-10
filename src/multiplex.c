#include <stdio.h>

#include "inputs.h"
#include "comms.h"

Message *multiplex(Inputs *inputs) {
    int i;
    Message *msg;

    if (poll(inputs->fds, POLL_ARRAY_SIZE, INFTIM) == -1) {
        perror(NULL);
        return NULL;
    }
    for (i = 0; i < POLL_ARRAY_SIZE; i++) {
        if (inputs->fds[i].revents & (POLLERR|POLLNVAL)) {
            perror(NULL);
            return NULL;
        }
        if (inputs->fds[i].revents & (POLLIN|POLLHUP)) {
            if (inputs_fd_is_queue(inputs, inputs->fds[i].fd)) {
                msg = event_queue_dequeue(inputs_retrieve_queue(inputs, inputs->fds[i].fd));
            } else {
                msg = receive(inputs->fds[i].fd);
                inputs_remove_fd(inputs, inputs->fds[i].fd);
            }
        }
    }
    return msg;
}
