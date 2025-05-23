#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "start.h"
#include "tsqueue.h"

TSQueue recv_queue, background_queue, foreground_queue;

struct ReceiverArgs {
	int fd;
	TSQueue *out_queue;
};

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
	if_error(tsqueue_init(&recv_queue), -1);
	if_error(tsqueue_init(&background_queue), -1);
	if_error(tsqueue_init(&foreground_queue), -1);
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
	int listenfd, connfd;
	struct sockaddr_in serv_addr, client_addr;
	socklen_t serv_addrlen;
	int barrier_error;
	struct ReceiverArgs *receiver_args;
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
		printf("FD %d opened\n", connfd);
		printf("Accepted connection from %s\n", inet_ntoa(client_addr.sin_addr));
		receiver_args = malloc(sizeof(*receiver_args));
		receiver_args->fd = connfd;
		receiver_args->out_queue = &background_queue;
		if_error(tsqueue_enqueue(&recv_queue, receiver_args), NULL);
	}
}

void *receiver(void *arg)
{
	struct ReceiverArgs *receiver_args;
	Message *msg;

	while (1) {
		receiver_args = tsqueue_dequeue(&recv_queue);
		msg = receive_message(receiver_args->fd);
		tsqueue_enqueue(receiver_args->out_queue, msg);
		free(receiver_args);
	}
}

Message *next_background_message(void)
{
	return (Message *) tsqueue_dequeue(&background_queue);
}

Message **foreground_messages(int *fds, int nfds) {
  Message **msglist;
  Message *msg;
  struct ReceiverArgs *receiver_args;
  int i, j;

  msglist = calloc(nfds, sizeof(*msglist));
  /* TODO: improve by selecting on fds and sending the readable ones first */
  for (i = 0; i < nfds; i++) {
    msglist[i] = NULL;
    receiver_args = malloc(sizeof(*receiver_args));
    receiver_args->fd = fds[i];
    receiver_args->out_queue = &foreground_queue;
    tsqueue_enqueue(&recv_queue, receiver_args);
  }
  for (i = 0; i < nfds; i++) {
    msg = tsqueue_dequeue(&foreground_queue);
    if (!msg)
      continue;
    for (j = 0; j < nfds; j++)
      if (msg->fd == fds[j])
        break;
    msglist[j] = msg;
  }

  return msglist;
}
