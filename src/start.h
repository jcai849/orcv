#include "comms.h"

#define BACKLOG 10

int start(const char *address, int port, int threads);
void *listener(void *arg);
void *receiver(void *arg);
Message *next_event(void);
