#ifndef MINQUEUE_H
#define MINQUEUE_H

#include <stdint.h>

typedef struct QueueNode QueueNode;
typedef struct QueueNode {
    uint64_t data;
    QueueNode* next;
} QueueNode;

typedef struct {
    uint64_t size;
    QueueNode *head;
} MinQueue;

//All return 0 on success and non-zero on failure.
int mq_addn(MinQueue *queue, QueueNode* node);
int mq_add(MinQueue *queue, void* alloc_addr, uint64_t data); 
int mq_remove(MinQueue *queue, QueueNode *node);
int mq_is_empty(MinQueue *queue);

#endif
