#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

int queue_init(struct Queue *queue)
{
    queue->head = NULL;
    queue->tail = NULL;
    return 0;
}

int queue_empty(struct Queue queue)
{
    return queue.head == NULL;
}

void *queue_dequeue(struct Queue *queue)
{
    void *pdata;
    struct Node *head;

    if (queue_empty(*queue))
        return NULL;
    pdata = queue->head->pdata;
    head = queue->head;
    queue->head = head->next;
    free(head);
    head = NULL;
    return pdata;
}

int queue_enqueue(struct Queue *queue, void *pdata)
{
    struct Node *node;

    if ((node = malloc(sizeof(*node))) == NULL) {
        perror(NULL);
        return 1;
    }
    node->next = NULL;
    node->pdata = pdata;

    if (queue_empty(*queue)) {
        queue->head = node;
    } else {
        queue->tail->next = node;
    }
    queue->tail = node;
    return 0;
}

int queue_destroy(struct Queue *queue)
{
    while (!queue_empty(*queue)) {
        queue_dequeue(queue);
    }
    return 0;
}
