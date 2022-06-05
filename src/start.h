#include "comms.h"

#define BACKLOG 10

int start(int, int);
void *listener(void *arg);
void *receiver(void *arg);
Message next_event(void);
