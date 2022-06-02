#include "start.h"

TSQueue *fd_queue, *event_queue;

struct ListenerArg {
	TSQueue *port_queue;
	int port;
};

int start(int port, int threads)
{
	TSQueue *port_queue;
	struct ListenerArg *listener_arg;
	struct ReceiverArg *receiver_arg;
	pthread_t thread;
	int *communicated_port;

	if (tsqueue_init(port_queue) != 0) {
		return NULL
	}
	if (tsqueue_init(fd_queue) != 0) {
		return NULL
	}
	if (tsqueue_init(event_queue) != 0) {
		return NULL
	}

	if ((listener_arg = malloc(sizeof(*listener_arg))) == NULL) {
		perror(NULL);
		return NULL;
	}
	listener_arg->port_queue = port_queue;
	listener_arg->port = port;
	if (pthread_create(&thread, NULL, &listener, listener_arg) != 0) {
		perror(NULL);
		return NULL;
	}
	
	if ((receiver_arg = malloc(sizeof(*receiver_arg))) == NULL) {
		perror(NULL);
		return NULL;
	}
	for (i = 0; i < threads; i++) {
		if (pthread_create(&thread, NULL, &receiver, NULL) != 0) {
		    perror(NULL);
		    return NULL;
		}
	}

	communicated_port = tsqueue_dequeue(port_queue);
	port = *communicated_port;
	free(communicated_port);
	communicated_port = NULL;
	if (tsqueue_destroy(port_queue) != 0) {
		return NULL
	}
	port_queue = NULL;

	return port;
}

void *listener(void *arg) {}
void *receiver(void *arg) {}
int send_message(int header_size, char *header, int payload_size, void *payload) {}
Message next_event(void) {}
