#include <assert.h>
#include "../src/comms.h"

/* just remember to close socks after use! */

void *sender(void *arg)
{
    send_data("localhost", 12345L, (Data *) arg);
    pthread_exit(NULL);
}

int main(void)
{
    Data data;
    int value[] = { 11, 22, 33, 44, 55 };
    pthread_t sender_thread;
    Inputs* inputs;

    data.data = value;
    data.size = 5;

    inputs = start(12345L, 4);
    pthread_create(&sender_thread, NULL, &sender, &data);
    event_queue_dequeue(inputs->queues->queue);
    return 0;
}
