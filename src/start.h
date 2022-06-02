#include "tsqueue.h"

typedef struct Message {
	int header_size;
	char *header;
	int payload_size;
	void *payload;
} Message;

int start(int, int);
void *listener(void *arg)
void *receiver(void *arg)
int send_message(int header_size, char *header, int payload_size, void *payload)
Message next_event(void)
