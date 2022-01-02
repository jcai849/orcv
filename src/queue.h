#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
    struct Node *next;
    void *pdata;
} Node;

typedef struct Queue {
    Node *head;
    Node *tail;
} Queue;

Queue *make_queue(void);
int queue_empty(Queue *);
void *pop_queue(Queue *);
int push_queue(Queue *, void *);
int rm_queue(Queue *);
