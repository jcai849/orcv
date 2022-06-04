#include "tsqueue.h"

#define MAX_SEND_SIZE (1024*1024*128)
#define BACKLOG 10
#define CONN_SLEEP 1

#ifdef NDEBUG
#define if_error(x, y) if(x) {perror("error"); return y;}
# else
#define if_error(x, y) x;
#endif

typedef struct Message {
	in_addr_t addr;
	in_port_t port;
	int header_size;
	char *header;
	int payload_size;
	void *payload;
} Message;

int start(int, int);
void *listener(void *arg);
int send_message(struct Message);
int send_data(int, void *, int);
void *receiver(void *arg);
Message receive(int fd);
Message next_event(void);
