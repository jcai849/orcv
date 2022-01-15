#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../src/multiplex.h"

#define PORT 12345L

/* just remember to close socks after use! */

void *data_sender(void *arg)
{
    send_data("localhost", PORT, (Data *) arg);
    pthread_exit(NULL);
}

int test_new_send(Inputs *inputs)
{
    Data data;
    char *value = "Hello there!";
    Message *msg, *msg2;
    pthread_t sender_thread;

    data.data = value;
    data.size = strlen(value);

    pthread_create(&sender_thread, NULL, &data_sender, &data);
    msg = multiplex(inputs);
    assert(!strncmp((char *) msg->data->data, value, data.size));
    pthread_create(&sender_thread, NULL, &data_sender, &data);
    msg2 = multiplex(inputs);
    assert(!strncmp((char *) msg2->data->data, value, data.size));
    return 0;
}

int main(void)
{
    Inputs* inputs;

    inputs = start(PORT, 4);
    test_new_send(inputs);
    return 0;
}
