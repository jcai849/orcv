#define MAX_SEND_SIZE (1024*1024*128)
#define CONN_SLEEP 1

#ifdef NDEBUG
#define if_error(x, y) x;
# else
#define if_error(x, y) if(x) {perror("error"); return y;}
#endif

typedef struct Message {
        in_addr_t addr;
        in_port_t port;
        int header_size;
        char *header;
        int payload_size;
        void *payload;
} Message;

in_port_t get_port(void);
in_addr_t get_address(void);
void set_port(in_port_t);
void set_address(in_addr_t);
Message *receive_message(int fd);
int receive_data(int, void *, int);
int send_message(struct Message);
int send_data(int, void *, int);
void delete_message(Message *msg);
