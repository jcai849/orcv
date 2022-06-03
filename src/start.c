#include "start.h"

TSQueue *fd_queue, *event_queue;

struct ListenerArg {
	char *hostname;
	int port;
};

int start(char *hostname, int port, int threads)
{
	TSQueue *port_queue;
	struct ListenerArg *listener_arg;
	struct ReceiverArg *receiver_arg;
	pthread_t thread;
	int *communicated_port;

	if (tsqueue_init(fd_queue) != 0) {
		return -1;
	}
	if (tsqueue_init(event_queue) != 0) {
		return -1;
	}

	if ((listener_arg = malloc(sizeof(*listener_arg))) == NULL) {
		perror(NULL);
		return -1;
	}
	listener_arg->hostname = hostname;
	listener_arg->port = port;
	if (pthread_create(&thread, NULL, &listener, listener_arg) != 0) {
		perror(NULL);
		return -1;
	}
	
	if ((receiver_arg = malloc(sizeof(*receiver_arg))) == NULL) {
		perror(NULL);
		return -1;
	}
	for (i = 0; i < threads; i++) {
		if (pthread_create(&thread, NULL, &receiver, NULL) != 0) {
		    perror(NULL);
		    return -1;
		}
	}

	return 0;
}

void *listener(void *arg)
{
    struct addrinfo hints, *res, *p;
    int addr_error;
    int sockfd, client_fd, *queued_fd;
    char *servname;
#ifdef ORCV_VERBOSE
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    char s[INET6_ADDRSTRLEN];
#endif
    int yes = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((servname = itoa(((struct ListenerArg *) arg)->port)) == NULL) {
        fprintf(stderr, "unable to convert port to string\n");
        return NULL;
    }
    if ((addr_error = getaddrinfo(NULL, servname, &hints, &res)) != 0) {
        fprintf(stderr, "%s\n", gai_strerror(addr_error));
        return NULL;
    }
    free(servname);

    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
            perror(NULL);
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
            perror(NULL);
            return NULL;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror(NULL);
            return NULL;
        }
        break;
    }
    freeaddrinfo(res);
    if (p == NULL) {
        fprintf(stderr, "bind failure\n");
        return NULL;
    }
    if (listen(sockfd, BACKLOG) == -1) {
        perror(NULL);
        return NULL;
    }

    while (1) {
#ifdef ORCV_VERBOSE
        addr_size = sizeof client_addr;
        client_fd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_size);
#else
        client_fd = accept(sockfd, NULL, NULL);
#endif
        if (client_fd == -1) {
            perror(NULL);
            continue;
        }
#ifdef ORCV_VERBOSE
        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr), s, sizeof(s));
        printf("got connection from %s\n", s);
#endif
        if ((queued_fd = malloc(sizeof(*queued_fd))) == NULL) {
            perror(NULL);
            return NULL;
        };
        *queued_fd = client_fd;
        if (tsqueue_enqueue(fd_queue, queued_fd) != 0) {
            fprintf(stderr, "enqueue issue\n");
            return NULL;
        }
    }
}

void *receiver(void *arg)
{
    int *client_fd
    int filter_error;
    struct Message *msg;

    while (1) {
        if ((client_fd = tsqueue_dequeue(((struct ReceiverArg *) arg)->ts_queue)) == NULL) {
            fprintf(stderr, "dequeueing NULL\n");
            return NULL;
        }
        if ((msg = receive(*client_fd)) == NULL) {
	    fprintf(stderr, "receive issue\n");
            return NULL;
        }
	close(*client_fd);
	*client_fd = -1;
	free(client_fd);
	if (tsqueue_enqueue(event_queue, msg) != 0) {
            fprintf(stderr, "enqueue issue\n");
	    return NULL;
	}
    }
}

Message receive(int fd)
{
	unsigned int header_len = 0, payload_len = 0;
	int n;

	n = recvfrom(fd, &header_len, sizeof(len), 0, from, fromlen)

}

int send_message(int header_size, char *header, int payload_size, void *payload) {}
Message next_event(void) {}
