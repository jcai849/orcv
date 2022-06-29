#include "comms.h"

#define BACKLOG 10

int start(const char *, int, int);
void *listener(void *arg);
void *receiver(void *arg);
Message *next_event(void);
