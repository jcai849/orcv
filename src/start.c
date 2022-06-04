#include "start.h"

TSQueue *fd_queue, *event_queue;
in_addr_t self_address;
in_port_t self_port;

int start(int port, int threads)
{
	struct ListenerArg *listener_arg;
	struct ReceiverArg *receiver_arg;
	pthread_t thread;
	pthread_barrier_t listener_barrier;
	pthread_barrierattr_t bar_attr;
	int *communicated_port;
	int barrier_error;

	self_port = htons(port);
	if_error(tsqueue_init(fd_queue), -1);
	if_error(tsqueue_init(event_queue), -1);
	if_error(pthread_barrierattr_init(&bar_attr), -1);
	if_error(pthread_barrier_init(&listener_barrier, &bar_attr, 2), -1);
	if_error(pthread_create(&thread, NULL, &listener, &listener_barrier), -1);
	for (i = 0; i < threads; i++) {
		if_error(pthread_create(&thread, NULL, &receiver, NULL), -1);
	}

	barrier_error = pthread_barrier_wait(&listener_barrier);
	if_error(barrier_error != 0 && barrier_error != PTHREAD_BARRIER_SERIAL_THREAD, -1);
	if_error(pthread_barrierattr_destroy(bar_attr), -1);
	if_error(pthread_barrier_destroy(&listener_barrier), -1);

	return 0;
}

void *listener(void *arg)
{
	int listenfd, connfd, *queuedfd;
	struct sockaddr_in serv_addr, ;
	socklen_t serv_addrlen;
	int barrier_error;
	
#ifdef ORCV_VERBOSE
	struct sockaddr_storage client_addr;
	socklen_t addr_size;
	char s[INET6_ADDRSTRLEN];
#endif
	int yes = 1;

	memset(&servaddr, 0, sizeof servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(self_port);
        if_error((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1, NULL);
        if_error(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1, NULL);
        if_error(bind(listenfd, (struct sockaddr *) &serv_addr,  sizeof(serv_addr)) == -1, NULL);
	if_error(listen(listenfd, BACKLOG) == -1, NULL);
	if_error(getsockname(listenfd, (struct sockaddr *) &serv_addr, &serv_addrlen), NULL);
	if_error(serv_addrlen != sizeof(serv_addr), NULL);
	self_address = htonl(serv_addr.sin_addr.s_addr);
	self_port = htons(serv_addr.sin_port);

	barrier_error = pthread_barrier_wait((pthread_barrier_t *) arg);
	if_error(barrier_error != 0 && barrier_error != PTHREAD_BARRIER_SERIAL_THREAD, NULL);
	arg = NULL;

	while (1) {
#ifdef ORCV_VERBOSE
		addr_size = sizeof client_addr;
		if_error((connfd = accept(listenfd, (struct sockaddr *) &client_addr, &addr_size)) == -1, NULL);
#else
		if_error((connfd = accept(listenfd, NULL, NULL)) == -1, NULL);
#endif
#ifdef ORCV_VERBOSE
		inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr), s, sizeof(s));
		printf("got connection from %s\n", s);
#endif
		if_error((queuedfd = malloc(sizeof(*queued_fd))) == NULL, NULL);
		*queuedfd = connfd;
		if_error(tsqueue_enqueue(fd_queue, queuedfd), NULL);
	}
}

void *receiver(void *arg)
{
	int *client_fd;
	Message *msg;

	while (1) {
		if_error(client_fd = tsqueue_dequeue(fd_queue), NULL);
		if_error((msg = receive_message(*client_fd)) == NULL, NULL);
		if_error(close(client_fd) == -1, NULL);
		*client_fd = -1;
		free(client_fd);
		if_error(tsqueue_enqueue(event_queue, msg), NULL);
	}
}

Message *next_event(void)
{
	return (Message *) tsqueue_dequeue(event_queue);
}

/* comms */


Message *receive_message(int fd)
{
        Message *msg;

        if_error((msg = malloc(sizeof(* msg))) == NULL, NULL);
        if_error(recv(fd, &msg->addr, sizeof msg->addr) != sizeof msg->addr, NULL);
        msg->addr = ntohl(msg->addr);
        if_error(recv(fd, &msg->port, sizeof msg->port) != sizeof msg->port, NULL);
        msg->port = ntohl(msg->port);
        if_error(recv(fd, &msg->header_size, sizeof msg->header_size) != sizeof msg->header_size, NULL);
        if_error((msg->header = malloc(msg->header_size)) == NULL, NULL);
        if_error(receive_data(fd, msg->header, msg->header_size), NULL);
        if_error(recv(fd, &msg->payload_size, sizeof msg->payload_size) != sizeof msg->payload_size, NULL);
        if_error((msg->payload = malloc(msg->payload_size)) == NULL, NULL);
        if_error(receive_data(fd, msg->payload, msg->payload_size), NULL);

        return msg;

}

int receive_data(int sockfd, void *data, int len)
{
        int i = 0, n, need;
        void *data;

        while (i < len) {
                need = (len - i > MAX_RECV_SIZE) ? MAX_RECV_SIZE : (len - i);
                n = recv(sockfd, data + i, need, 0);
                if (n < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                        fprintf(stderr, "Read error on descriptor %d: %s", sockfd, strerror(errno));
                        if_error(close(sockfd) == -1, -1);
                        sockfd = -1;
                        return -1;
                } else if (n == 0) {
                        fprintf(stderr, "Connection closed on descriptor %d before all data was received", sockfd);
                        if_error(close(sockfd) == -1, -1);
                        sockfd = -1;
                        return -1;
                }
                i += n;
        }
        return 0;
}

int send_message(Message msg)
{
        int sockfd;
        int no_connect, reconnections = 0;
        struct sockaddr_in servaddr;

        memset(&servaddr, 0, sizeof servaddr);
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(msg.port);
        servaddr.sin_addr = htons(msg.addr);
        do {
                if_error((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1, -1);
                no_connect = connect(sockfd, (sockaddr *) &servaddr, sizeof(servaddr));
                if (no_connect) {
                        perror(NULL);
                        if_error(fprintf(stderr, "Attempting reconnect to %s port %d (attempt #%d)\n", addr, port, ++reconnections) < 0,
 -1);
                        if_error(close(sockfd) == -1, -1);
                        sleep(CONN_SLEEP);
                }
        } while (no_connect);

        if_error(send(sockfd, &htonl(self_addr), sizeof self_addr, 0) != sizeof self_addr, -1);
        if_error(send(sockfd, &htons(self_port), sizeof self_port, 0) != sizeof self_port, -1);
        if_error(send(sockfd, &msg.header_size, sizeof msg.header_size, 0) != sizeof msg.header_size, -1);
        if_error(send_data(sockfd, msg.header, header_size, 0) == -1, -1);
        if_error(send(sockfd, &msg.payload_size, sizeof msg.payload_size, 0) != sizeof msg.payload_size, -1);
        if_error(send_data(sockfd, msg.payload, payload_size, 0) == -1, -1);

        if_error(close(sockfd) == -1, -1);
        sockfd = -1;

        return 0;
}

int send_data(int sockfd, void *data, int len)
{
        int i = 0, need, n;

        while (i < len) {
                need = (len - i > MAX_SEND_SIZE) ? MAX_SEND_SIZE : (len - i);
                n = send(sockfd, data + i, need, 0);
                if (n < 1) {
                        perror(NULL);
                        if_error(close(sockfd) == -1, -1);
                        if_error(fprintf(stderr, "Failed to write (n=%d of %d) %s\n", n, need, (n == -1 && errno) ? strerror(errno) : ""
) < 0, -1);
                        return -1;
                }
                i += n;
        }
        return 0;
}

