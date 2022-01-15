#include "../src/comms.h"
#include <stdlib.h>
#include <stdio.h>

#define PORT 12345

struct ListenerArg {
    int port;
    TSQueue *ts_queue;
};

int main(void)
{
    struct ListenerArg *listener_arg;
    TSQueue *ts_queue;

    if ((ts_queue = malloc(sizeof(*ts_queue))) == NULL) {
        perror(NULL);
    };
    tsqueue_init(ts_queue);
    if ((listener_arg = malloc(sizeof(*listener_arg))) == NULL) {
        perror(NULL);
    }
    listener_arg->port = PORT;
    listener_arg->ts_queue = ts_queue;
    if (listener(listener_arg) == NULL) {
        return 1;
    }
    return 0;
}
