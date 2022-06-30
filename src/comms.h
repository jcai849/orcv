#define MAX_SEND_SIZE (1024*1024*128)
#define MAX_RECV_SIZE (1024*1024*128)
#define CONN_SLEEP 1
#include <stdio.h>
#include <netinet/in.h>

/*
#ifdef DNDEBUG
#define if_error(x, y) x
# else
*/
#define if_error(x, y) if(x) {perror("error"); return y;}
/*
#endif
*/

typedef struct Message {
	int fd;
        in_addr_t addr;
        in_port_t port;
        int header_size;
        char *header;
        int payload_size;
        void *payload;
} Message;

in_port_t get_port(void);
in_addr_t get_address(void);
void set_port(in_port_t port);
void set_address(in_addr_t addr);
in_addr_t first_avail_iface(void);
in_addr_t address_from_string(const char *address, int port);
Message *receive_message(int fd);
int receive_data(int sockfd, void *data, int len);
int get_socket(int addr, int port);
int send_socket(int sockfd, int header_size, char *header, int payload_size, unsigned char *payload);
int send_message(Message *msg);
int send_data(int sockfd, const void *data, int len);
void delete_message(Message *msg);
