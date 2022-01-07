typedef struct Node {
    struct Node *next;
    void *pdata;
} Node;

typedef struct Queue {
    Node *head;
    Node *tail;
} Queue;

int queue_init(Queue *);
int queue_empty(Queue);
void *queue_dequeue(Queue *);
int queue_enqueue(Queue *, void *);
int queue_destroy(Queue *);
