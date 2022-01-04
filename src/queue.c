#include "queue.h"

struct Queue *make_queue(void)
{
    struct Queue *queue;
    if ((queue = malloc(sizeof(*queue))) == NULL) {
        perror(NULL);
        abort();
    }
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

int queue_empty(struct Queue *queue)
{
    return queue->head == NULL;
}

void *pop_queue(struct Queue *queue)
{
    void *pdata;
    struct Node *head;

    if (queue_empty(queue))
        return NULL;
    pdata = queue->head->pdata;
    head = queue->head;
    queue->head = head->next;
    free(head);
    head = NULL;
    return pdata;
}

int push_queue(struct Queue *queue, void *pdata)
{
    struct Node *node;

    if ((node = malloc(sizeof(*node))) == NULL) {
        perror(NULL);
        abort();
    }
    node->next = NULL;
    node->pdata = pdata;

    if (queue_empty(queue)) {
        queue->head = node;
    } else {
        queue->tail->next = node;
    }
    queue->tail = node;
    return 0;
}

int rm_queue(struct Queue *queue)
{
    while (!queue_empty(queue)) {
        pop_queue(queue);
    }
    free(queue);
    return 0;
}
