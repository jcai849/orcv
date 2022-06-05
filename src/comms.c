#include <netinet/in.h>
#include <stdlib.h>
#include "comms.h"

in_addr_t self_address; /* both stored as  */
in_port_t self_port;    /* host byte order */

in_port_t get_port(void)
{
	return self_port;
}

in_addr_t get_address(void)
{
	return self_address;
}

void set_port(in_port_t port)
{
	self_port = port;
}

void set_address(in_addr_t addr)
{
	self_address = addr;
}

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

void delete_message(Message *msg) {
        free(msg->header);
        free(msg->payload);
        free(msg);
}
