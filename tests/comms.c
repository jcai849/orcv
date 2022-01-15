#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/comms.h"

#define PORT 12345L

/* just remember to close socks after use! */

void *sender(void *arg)
{
    send_data("localhost", PORT, (Data *) arg);
    pthread_exit(NULL);
}

int main(void)
{
    Data data;
    char *value = "Hello there!";

    pthread_t sender_thread;
    Inputs* inputs;
    Message *msg, *msg2;

    data.data = value;
    data.size = strlen(value);

    inputs = start(PORT, 4);
    pthread_create(&sender_thread, NULL, &sender, &data);
    msg = event_queue_dequeue(inputs->queues->queue);
    assert(!strncmp((char *) msg->data->data, value, data.size));
    pthread_create(&sender_thread, NULL, &sender, &data);
    msg2= event_queue_dequeue(inputs->queues->queue);
    assert(!strncmp((char *) msg2->data->data, value, data.size));
    return 0;
}
