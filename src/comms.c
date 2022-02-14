#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "comms.h"

#define MAX_RECV_SIZE (1024*1024*128)
#define MAX_SEND_SIZE (1024*1024*128)
#define BACKLOG 10

struct ListenerArg {
    int port;
    TSQueue *ts_queue;
};

struct ReceiverArg {
    int (*filter)(struct Message *);
    TSQueue *ts_queue;
    EventQueue *event_queue;
};

char *itoa(int num) // Auto-allocates; Must free returned val
{
    int length;
    char *string_form;

    if ((length = snprintf(NULL, 0, "%d", num)) < 0) {
        perror(NULL);
        return NULL;
    }
    if ((string_form = malloc(length + 1)) == NULL) {
        perror(NULL);
        return NULL;
    }
    if ((snprintf(string_form, length + 1, "%d", num)) < 0) {
        perror(NULL);
        return NULL;
    }
    return string_form;
}


Message *receive(int fd)
{
    unsigned int len = 0, i = 0;
    unsigned char *payload;
    int n, need;
    struct Message *msg;
    struct Data *data;

    n = read(fd, &len, sizeof(len));
    if (n != sizeof(len) || len == 0) {
      fprintf(stderr, "Header read error on descriptor %d", fd);
      close(fd);
      fd = -1;
      return NULL;
    } else {
        if ((payload = malloc(len)) == NULL) {
            perror(NULL);
            return NULL;
        }
        while (i < len) {
            need = (len - i > MAX_RECV_SIZE) ? MAX_RECV_SIZE : (len - i);
            n = read(fd, payload + i, need);
            if (n < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    continue;
                }
                close(fd);
                fd = -1;
                fprintf(stderr, "Read error on descriptor %d: %s", fd, strerror(errno));
                return NULL;
            } else if (n == 0) {
                close(fd);
                fd = -1;
                fprintf(stderr, "Connection closed on descriptor %d before all data was received", fd);
                return NULL;
            }
            i += n;
        }
    }
    if ((data = malloc(sizeof(*data))) == NULL) {
        perror(NULL);
        return NULL;
    }
    data->data = payload;
    data->size = len;
    if ((msg = malloc(sizeof(*msg))) == NULL) {
        perror(NULL);
        return NULL;
    }
    msg->data = data;
    msg->connection = fd;

    return msg;
};

int send_message(struct Message *msg) {
    int i = 0, need, n;
    int len = msg->data->size;
    int sockfd = msg->connection;

    if (write(sockfd, &len, sizeof(len)) != sizeof(len)) {
        close(sockfd);
        msg->connection = -1;
        perror(NULL);
        fprintf(stderr, "Failed to write header\n");
        return -1;
    }
    while (i < len) {
        need = (len - i > MAX_SEND_SIZE) ? MAX_SEND_SIZE : (len - i);
        n = write(sockfd, msg->data->data + i, need);
        if (n < 1) {
          close(sockfd);
          msg->connection = -1;
          fprintf(stderr, "Failed to write (n=%d of %d) %s\n", n, need, (n == -1 && errno) ? strerror(errno) : "");
          perror(NULL);
          return -1;
        }
        i += n;
    }
    return 0;
}

int send_data(const char *addr, int port, Data *data)
{
    struct addrinfo hints, *res;
    int addr_error, retcode;
    int sockfd;
    char *string_port;
    Message msg;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    if ((string_port = itoa(port)) == NULL) {
        fprintf(stderr, "unable to convert port to string\n");
        return -1;
    };
    addr_error = getaddrinfo(addr, string_port, &hints, &res);
    if  (addr_error) {
        fprintf(stderr, "%s\n", gai_strerror(addr_error));
        return -1;
    }
    free(string_port);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror(NULL);
        return -1;
    }
    retcode = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (retcode == -1) {
        perror(NULL);
        return -1;
    }

    msg.connection = sockfd;
    msg.data = data;
    if (send_message(&msg) != 0) {
        return -1;
    }

    return sockfd;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *listener(void *arg)
{
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res, *p;
    int addr_error;
    int sockfd, client_fd, *queued_fd;
    char *servname;
    // char s[INET6_ADDRSTRLEN];
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
    if (addr_error) {
        fprintf(stderr, "%s\n", gai_strerror(addr_error));
    }
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
        addr_size = sizeof client_addr;
        client_fd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_size);
        if (client_fd == -1) {
            perror(NULL);
            continue;
        }
        // inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr), s, sizeof(s));
        // printf("got connection from %s\n", s);
        if ((queued_fd = malloc(sizeof(*queued_fd))) == NULL) {
            perror(NULL);
            return NULL;
        };
        *queued_fd = client_fd;
        if (tsqueue_enqueue(((struct ListenerArg *) arg)->ts_queue, queued_fd) != 0) {
            fprintf(stderr, "enqueue issue\n");
            return NULL;
        }
    }
}


void *receiver(void *arg)
{
    int client_fd, *pclient_fd;
    int filter_error;
    struct Message *msg;

    while (1) {
        if ((pclient_fd = tsqueue_dequeue(((struct ReceiverArg *) arg)->ts_queue)) == NULL) {
            fprintf(stderr, "dequeueing NULL\n");
            return NULL;
        }
        client_fd = *pclient_fd;
        if ((msg = receive(client_fd)) == NULL) {
            return NULL;
        }
	if (((struct ReceiverArg *) arg)->filter != NULL) {
		filter_error = (*((struct ReceiverArg *) arg)->filter)(msg);
		if (filter_error) {
			fprintf(stderr, "filter error");
			return NULL;
		}
	}
        free(pclient_fd);
        if (event_queue_enqueue(((struct ReceiverArg *) arg)->event_queue, msg) != 0) {
            return NULL;
        }
    }
}

Inputs *start(int port, int (*filter)(struct Message *), int threads)
{
    struct ListenerArg *listener_arg;
    struct ReceiverArg *receiver_arg;
    TSQueue *ts_queue;
    EventQueue *event_queue;
    pthread_t thread;
    int i;
    Inputs *inputs;

    if ((ts_queue = malloc(sizeof(*ts_queue))) == NULL) {
        perror(NULL);
        return NULL;
    }
    if (tsqueue_init(ts_queue) != 0) {
        return NULL;
    }
    if ((event_queue = malloc(sizeof(*event_queue))) == NULL) {
        perror(NULL);
    }
    if (event_queue_init(event_queue) != 0) {
        return NULL;
    }

    if ((listener_arg = malloc(sizeof(*listener_arg))) == NULL) {
        perror(NULL);
        return NULL;
    }
    listener_arg->port = port;
    listener_arg->ts_queue = ts_queue;
    if ((receiver_arg = malloc(sizeof(*receiver_arg))) == NULL) {
        perror(NULL);
        return NULL;
    }
    receiver_arg->ts_queue = ts_queue;
    receiver_arg->event_queue = event_queue;
    receiver_arg->filter = filter;

    if (pthread_create(&thread, NULL, &listener, listener_arg) != 0) {
        perror(NULL);
        return NULL;
    }
    for (i = 0; i < threads; i++) {
        if (pthread_create(&thread, NULL, &receiver, receiver_arg) != 0) {
            perror(NULL);
            return NULL;
        }
    }

    if ((inputs = malloc(sizeof(*inputs))) == NULL) {
        perror(NULL);
        return NULL;
    };
    if (inputs_init(inputs) != 0) {
        return NULL;
    }
    if (inputs_insert_queue(inputs, event_queue) != 0) {
        return NULL;
    }

    return inputs;
}
