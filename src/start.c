#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "start.h"
#include "tsqueue.h"

TSQueue fd_queue, event_queue;

int start(const char *address, int port, int threads)
{
	pthread_t thread;
	pthread_barrier_t listener_barrier;
	pthread_barrierattr_t bar_attr;
	int barrier_error;
	int i;
	in_addr_t addr;

	set_port(port);
	addr = address ? address_from_string(address, port) : first_avail_iface();
	if (addr == -1) return -1;
	set_address(addr);
	if_error(tsqueue_init(&fd_queue), -1);
	if_error(tsqueue_init(&event_queue), -1);
	if_error(pthread_barrierattr_init(&bar_attr), -1);
	if_error(pthread_barrier_init(&listener_barrier, &bar_attr, 2), -1);
	if_error(pthread_create(&thread, NULL, &listener, &listener_barrier), -1);
	for (i = 0; i < threads; i++) {
		if_error(pthread_create(&thread, NULL, &receiver, NULL), -1);
	}

	barrier_error = pthread_barrier_wait(&listener_barrier);
	if_error(barrier_error != 0 && barrier_error != PTHREAD_BARRIER_SERIAL_THREAD, -1);
	if_error(pthread_barrierattr_destroy(&bar_attr), -1);
	if_error(pthread_barrier_destroy(&listener_barrier), -1);

	return 0;
}

void *listener(void *arg)
{
	int listenfd, connfd, *queuedfd;
	struct sockaddr_in serv_addr, client_addr;
	socklen_t serv_addrlen;
	int barrier_error;
	int yes = 1;
	socklen_t client_addrlen = sizeof client_addr;

	memset(&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(get_address());
	serv_addr.sin_port = htons(get_port());
        if_error((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1, NULL);
        if_error(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1, NULL);
        if_error(bind(listenfd, (struct sockaddr *) &serv_addr,  sizeof(serv_addr)) == -1, NULL);
	if_error(listen(listenfd, BACKLOG) == -1, NULL);

	memset(&serv_addr, 0, sizeof serv_addr);
	serv_addrlen = sizeof serv_addr;
	if_error(getsockname(listenfd, (struct sockaddr *) &serv_addr, &serv_addrlen), NULL);
	if_error(serv_addrlen != sizeof(serv_addr), NULL);
	printf("server bound to address %s at port %d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
	set_address(ntohl(serv_addr.sin_addr.s_addr));
	set_port(ntohs(serv_addr.sin_port));

	barrier_error = pthread_barrier_wait((pthread_barrier_t *) arg);
	if_error(barrier_error != 0 && barrier_error != PTHREAD_BARRIER_SERIAL_THREAD, NULL);
	arg = NULL;

	while (1) {
		if_error((connfd = accept(listenfd, (struct sockaddr *) &client_addr, &client_addrlen)) == -1, NULL);
		printf("FD %d opened", connfd);
		printf("Accepted connection from %s\n", inet_ntoa(client_addr.sin_addr));
		if_error((queuedfd = malloc(sizeof(*queuedfd))) == NULL, NULL);
		*queuedfd = connfd;
		if_error(tsqueue_enqueue(&fd_queue, queuedfd), NULL);
	}
}

void *receiver(void *arg)
{
	int *client_fd;
	Message *msg;

	while (1) {
		if_error((client_fd = tsqueue_dequeue(&fd_queue)) == NULL, NULL);
		if_error((msg = receive_message(*client_fd)) == NULL, NULL);
		free(client_fd);
		if_error(tsqueue_enqueue(&event_queue, msg), NULL);
	}
}

Message *next_event(void)
{
	return (Message *) tsqueue_dequeue(&event_queue);
}
