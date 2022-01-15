#include "../src/comms.h"
#include <stdlib.h>
#include <stdio.h>

#define PORT 12345

struct ListenerArg {
    int port;
    TSQueue *ts_queue;
};

struct ReceiverArg {
    TSQueue *ts_queue;
    EventQueue *event_queue;
};

int main(void)
{
    struct ListenerArg *listener_arg;
    struct ReceiverArg *receiver_arg;
    TSQueue *ts_queue;
    EventQueue *event_queue;
    Message *msg;

    if ((ts_queue = malloc(sizeof(*ts_queue))) == NULL) {
        perror(NULL);
    };
    tsqueue_init(ts_queue);
    if ((listener_arg = malloc(sizeof(*listener_arg))) == NULL) {
        perror(NULL);
    }
    listener_arg->port = PORT;
    listener_arg->ts_queue = ts_queue;
    listener(listener_arg);
    if ((receiver_arg = malloc(sizeof(*receiver_arg))) == NULL) {
        perror(NULL);
    }
    if ((event_queue = malloc(sizeof(*event_queue))) == NULL) {
        perror(NULL);
    }
    event_queue_init(event_queue);
    receiver_arg->ts_queue = ts_queue;
    receiver_arg->event_queue = event_queue;
    receiver(receiver_arg);
    msg = event_queue_dequeue(event_queue);
    printf("receiving message: %s\n", (char *) msg->data->data);
    return 0;
}
