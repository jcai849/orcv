#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "inputs.h"
#include "comms.h"

#define NTHREADS 4
#define MAX_RECV_SIZE (1024*1024*128)
#define MAX_SEND_SIZE (1024*1024*128)
#define BACKLOG 10

struct ListenerArg {
    int port;
    TSQueue *ts_queue;
};

struct ReceiverArg {
    TSQueue *ts_queue;
    EventQueue *event_queue;
};

char *itoa(int num)
{
    char *string_form;
    int length;

    length = snprintf(NULL, 0, "%d", num);
    if ((string_form = malloc(length + 1)) == NULL) {
        perror(NULL);
        abort();
    }
    snprintf(string_form, length + 1, "%d", num);
    return string_form;
}


Message *receive(int fd)
{
    unsigned int len = 0, i = 0;
    unsigned char *payload;
    int n, need;
    struct Message *msg;

    n = recv(fd, &len, sizeof(len), 0);
    if (n != sizeof(len) || len == 0) {
      close(fd);
      fd = -1;
      fprintf(stderr, "Header read error on descriptor %d", fd);
      abort();
        } else {
            if ((payload = malloc(len)) == NULL) {
                perror(NULL);
                abort();
            }
            while (i < len) {
                need = (len - i > MAX_RECV_SIZE) ? MAX_RECV_SIZE : (len - i);
                n = recv(fd, payload + i, need, 0);
                if (n < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        continue;
                    }
                    close(fd);
                    fd = -1;
                    fprintf(stderr, "Read error on descriptor %d: %s", fd, strerror(errno));
                    abort();
                } else if (n == 0) {
                    close(fd);
                    fd = -1;
                    fprintf(stderr, "Connection closed on descriptor %d before all data was received", fd);
                    abort();
                }
                i += n;
            }
        }
        if ((msg = malloc(sizeof(Message))) == NULL) {
            perror(NULL);
            abort();
        }
        msg->data->data = payload;
        msg->data->size = len;
        msg->connection = fd;

        return msg;
};

int send_message(struct Message *msg) {
    int i = 0, need, n;
    int len = msg->data->size;
    int sockfd = msg->connection;

    if (send(sockfd, &len, sizeof(len), 0) != sizeof(len)) {
        close(sockfd);
        msg->connection = -1;
        fprintf(stderr, "Failed to write header\n");
        abort();
    }
    while (i < len) {
        need = (len - i > MAX_SEND_SIZE) ? MAX_SEND_SIZE : (len - i);
        n = send(sockfd, msg->data->data + i, need, 0);
        if (n < 1) {
          close(sockfd);
          msg->connection = -1;
          fprintf(stderr, "Failed to write (n=%d of %d) %s\n", n, need, (n == -1 && errno) ? strerror(errno) : "");
          abort();
        }
        i += n;
    }
    return 0;
}

int send_data(char *addr, int port, Data *data) // must free data afterwards
{
    struct addrinfo hints, *res;
    int addr_error, retcode;
    int sockfd;
    char *string_port;
    Message *msg;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    string_port = itoa(port);
    addr_error = getaddrinfo(addr, string_port, &hints, &res);
    if  (addr_error) {
        fprintf(stderr, "%s\n", gai_strerror(addr_error));
        abort();
    }
    free(string_port);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror(NULL);
        abort();
    }
    retcode = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (retcode == -1) {
        perror(NULL);
        abort();
    }

    if ((msg = malloc(sizeof(msg))) == NULL) {
        perror(NULL);
        abort();
    }
    msg->connection = sockfd;
    msg->data = data;
    send_message(msg);
    free(msg);

    return sockfd;
}

void *listener(void *arg)
{
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int addr_error;
    int port_length;
    int sockfd, client_fd, *queued_fd;
    char *servname;

    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    servname = itoa(((struct ListenerArg *) arg)->port);

    addr_error = getaddrinfo(NULL, servname, &hints, &res);
    free(servname);
    if (addr_error) {
        fprintf(stderr, "%s\n", gai_strerror(addr_error));
        abort();
    }
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror(NULL);
        close(sockfd);
        abort();
    }
    if (sockfd == 0) {
        perror(NULL);
        abort();
    }
    freeaddrinfo(res);
    (void) listen(sockfd, BACKLOG);
    addr_size = sizeof client_addr;

    while (1) {
        client_fd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_size);
        if (client_fd == -1) {
            perror(NULL);
            abort();
        }
        queued_fd = malloc(sizeof(int));
        push_ts_queue(((struct ListenerArg *) arg)->ts_queue, queued_fd);
    }
    free(((struct ListenerArg *) arg));
}


void *receiver(void *arg)
{
    int client_fd, *pclient_fd;
    struct Message *msg;

    while (1) {
        pclient_fd = pop_ts_queue(((struct ReceiverArg *) arg)->ts_queue);
        client_fd = *pclient_fd;
        free(pclient_fd);
        msg = receive(client_fd);
        push_event_queue(((struct ReceiverArg *) arg)->event_queue, msg);
    }
    free(arg);
}

Inputs *start(int port)
{
    struct ListenerArg *listener_arg;
    struct ReceiverArg *receiver_arg;
    TSQueue *ts_queue;
    EventQueue *event_queue;
    pthread_t listener_thread, receiver_thread[NTHREADS];
    int i;
    Inputs *inputs;

    ts_queue = make_ts_queue();
    event_queue = make_event_queue();

    if ((listener_arg = malloc(sizeof(listener_arg))) == NULL) {
        perror(NULL);
        abort();
    }
    listener_arg->port = port;
    listener_arg->ts_queue = ts_queue;
    if ((receiver_arg = malloc(sizeof(receiver_arg))) == NULL) {
        perror(NULL);
        abort();
    }
    receiver_arg->ts_queue = ts_queue;
    receiver_arg->event_queue = event_queue;

    pthread_create(&listener_thread, NULL, listener, listener_arg);
    for (i = 0; i < NTHREADS; i++) {
        pthread_create(&(receiver_thread[i]), NULL, receiver, receiver_arg);
    }

    inputs = make_inputs();
    add_queue(inputs, event_queue);

    return inputs;
}
