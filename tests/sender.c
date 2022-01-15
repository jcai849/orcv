#include "../src/comms.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PORT 12345

int main(void)
{
    Data data;
    char *value = "Hello there!";

    data.data = value;
    data.size = strlen(value);

    printf("sending message: %s\n", value);

    send_data("127.0.0.1", PORT, &data);
    char *value2 = "goodbye!";

    data.data = value2;
    data.size = strlen(value2);

    printf("sending message: %s\n", value2);

    send_data("127.0.0.1", PORT, &data);
    return 0;
}
