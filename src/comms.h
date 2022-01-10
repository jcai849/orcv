#include "inputs.h"

typedef struct Data {
    void *data;
    int size;
} Data;

typedef struct Message {
    Data *data;
    int connection;
} Message;

Message *receive(int);
int send_data(char *addr, int port, struct Data *);
int send_message(struct Message *);
Inputs *start(int, int);
