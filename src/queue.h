typedef struct Node {
    struct Node *next;
    void *pdata;
} Node;

typedef struct Queue {
    Node *head;
    Node *tail;
} Queue;

int queue_init(Queue *queue);
int queue_empty(Queue queue);
void *queue_dequeue(Queue *queue);
int queue_enqueue(Queue *queue, void *pdata);
int queue_destroy(Queue *queue);
