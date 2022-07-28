#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <netdb.h>
#include <Rinternals.h>
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

in_addr_t first_avail_iface(void)
{
        struct ifaddrs *ifap, *ifa;
        in_addr_t addr;

	getifaddrs(&ifap);
        for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
                if (ifa->ifa_addr &&
                    ifa->ifa_addr->sa_family == AF_INET) /* then we can cast to sockaddr_in: */
                        if(((struct sockaddr_in *) ifa->ifa_addr)->sin_addr.s_addr != htonl(INADDR_LOOPBACK) &&
                           ((struct sockaddr_in *) ifa->ifa_addr)->sin_addr.s_addr != htonl(INADDR_ANY)) {
                                break;
                }
                if (!ifa->ifa_next) {
                        perror("No interface found\n");
                        return -1;
                }
        }
        addr = ((struct sockaddr_in *) ifa->ifa_addr)->sin_addr.s_addr;
        freeifaddrs(ifap);
	return ntohl(addr);
}

in_addr_t address_from_string(const char *address, int port)
{
	struct addrinfo hints, *result;
	char service[16+1];
	in_addr_t addr;
	int error;

	memset(&hints, 0, sizeof hints);
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	snprintf(service, 16+1, "%d", port);

	error = getaddrinfo(address, service, &hints, &result);
	if (error) {
		perror(gai_strerror(error));
		freeaddrinfo(result);
		return -1;
	}
	addr = ((struct sockaddr_in *) result->ai_addr)->sin_addr.s_addr;
	freeaddrinfo(result);

	return ntohl(addr);
}

Message *receive_message(int fd)
{
        Message *msg;
        struct sockaddr_in client_name;
        socklen_t client_namelen = sizeof client_name;

	getpeername(fd, (struct sockaddr *) &client_name, &client_namelen);
	printf("Attempting to receive message from %s port %d over FD %d\n",
	       inet_ntoa(client_name.sin_addr), ntohs(client_name.sin_port), fd);

        if_error((msg = malloc(sizeof(*msg))) == NULL, NULL);
	msg->fd = fd;
        if_error(recv(fd, &msg->addr, sizeof msg->addr, 0) != sizeof msg->addr, NULL);
        msg->addr = ntohl(msg->addr);
        if_error(recv(fd, &msg->port, sizeof msg->port, 0) != sizeof msg->port, NULL);
        msg->port = ntohs(msg->port);
        if_error(recv(fd, &msg->header_size, sizeof msg->header_size, 0) != sizeof msg->header_size, NULL);
        if_error((msg->header = calloc(msg->header_size, sizeof &msg->header)) == NULL, NULL);
        if_error(receive_data(fd, msg->header, msg->header_size), NULL);
        if_error(recv(fd, &msg->payload_size, sizeof msg->payload_size, 0) != sizeof msg->payload_size, NULL);
        if_error((msg->payload = malloc(msg->payload_size)) == NULL, NULL);
        if_error(receive_data(fd, msg->payload, msg->payload_size), NULL);

	printf("Received message from %s port %d over fd %d with header \"%s\"\n",
	       inet_ntoa(client_name.sin_addr), ntohs(client_name.sin_port), fd, msg->header);

        return msg;

}

int receive_data(int sockfd, void *data, int len)
{
        int i = 0, n, need;

        while (i < len) {
                need = (len - i > MAX_RECV_SIZE) ? MAX_RECV_SIZE : (len - i);
                n = recv(sockfd, data + i, need, 0);
                if (n < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
				R_CheckUserInterrupt();
				continue;
			}
                        fprintf(stderr, "Read error on descriptor %d: %s", sockfd, strerror(errno));
                        if_error(close(sockfd) == -1, -1);
			printf("FD %d closed", sockfd);
                        sockfd = -1;
                        return -1;
                } else if (n == 0) {
                        fprintf(stderr, "Connection closed on descriptor %d before all data was received", sockfd);
                        if_error(close(sockfd) == -1, -1);
			printf("FD %d closed", sockfd);
                        sockfd = -1;
                        return -1;
                }
                i += n;
        }
        return 0;
}

int get_socket(int addr, int port)
{
        int sockfd;
        int no_connect, reconnections = 0;
        struct sockaddr_in servaddr;

        memset(&servaddr, 0, sizeof servaddr);
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        servaddr.sin_addr.s_addr = htonl(addr);
        do {
                if_error((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1, -1);
		printf("FD %d opened", sockfd);
                no_connect = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
                if (no_connect) {
                        perror(NULL);
                        if_error(fprintf(stderr, "Attempting reconnect to %s port %d (attempt #%d)\n",
					 inet_ntoa(servaddr.sin_addr), port, ++reconnections) < 0, -1);
                        if_error(close(sockfd) == -1, -1);
			printf("FD %d closed", sockfd);
                        sleep(CONN_SLEEP);
                }
        } while (no_connect);

        return sockfd;
}

int send_socket(int sockfd, int header_size, char *header, int payload_size, unsigned char *payload)
{
	Message msg;
	
	msg.fd = sockfd;
	msg.addr = htonl(get_address());
	msg.port = htons(get_port());
	msg.header_size = header_size;
	msg.header = header;
	msg.payload_size = payload_size;
	msg.payload = payload;

	return send_message(&msg);
}

int send_message(Message *msg)
{
	int fd = msg->fd;
	struct sockaddr_in peername;
	socklen_t namelen = sizeof peername;

	if_error(getpeername(fd, (struct sockaddr *) &peername, &namelen), -1);
	
	printf("Attempting to send message with header \"%s\" to address %s port %d over fd %d\n",
		msg->header, inet_ntoa(peername.sin_addr), ntohs(peername.sin_port), fd);

        if_error(send(fd, &msg->addr, sizeof msg->addr, 0) != sizeof msg->addr, -1);
        if_error(send(fd, &msg->port, sizeof msg->port, 0) != sizeof msg->port, -1);
        if_error(send(fd, &msg->header_size, sizeof msg->header_size, 0) != sizeof msg->header_size, -1);
        if_error(send_data(fd, msg->header, msg->header_size) == -1, -1);
        if_error(send(fd, &msg->payload_size, sizeof msg->payload_size, 0) != sizeof msg->payload_size, -1);
        if_error(send_data(fd, msg->payload, msg->payload_size) == -1, -1);
	
	printf("Message sent to %s port %d over fd %d\n", inet_ntoa(peername.sin_addr), ntohs(peername.sin_port), fd);

        return 0;
}

int send_data(int sockfd, const void *data, int len)
{
        int i = 0, need, n;

        while (i < len) {
                need = (len - i > MAX_SEND_SIZE) ? MAX_SEND_SIZE : (len - i);
                n = send(sockfd, data + i, need, 0);
                if (n < 1) {
                        perror(NULL);
                        if_error(close(sockfd) == -1, -1);
			printf("FD %d closed", sockfd);
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
