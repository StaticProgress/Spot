#include <stdio.h>
#include "minqueue.h"


//We sort our QueueNode's based on the their address.
//Function assumes that Node has already been allocated.
int mq_addn(MinQueue *queue, QueueNode* node)
{
    if(!queue || !node)
        return 1;
    QueueNode *curr, *last = NULL;
    for(curr = queue->head; curr != NULL && curr < node; last = curr, curr = curr->next);
    //If it comes before the head
    if(!last) {
        node->next = queue->head;
        queue->head = node;
    } else {
        last->next = node;
        node->next = curr;
    }
    queue->size++;
    return 0;
}

//Allocates the struct destructively on the heap!
int mq_add(MinQueue *queue, void* alloc_addr, uint64_t data) 
{
    if(!queue)
        return 1;
    //Allocate at the given addr
    QueueNode *node = (QueueNode*) alloc_addr;
    node->data = data;
    return mq_addn(queue, node);
}

int mq_remove(MinQueue *queue, QueueNode *node)
{
    if(!queue || !node)
        return 1;
    QueueNode *curr, *last = NULL;
    //Search the queue for this node
    for(curr = queue->head; curr != node; last = curr, curr = curr->next);
    //Didn't find it
    if(!curr)
        return 2;
    //Else its the first element
    else if(!last)
        //We don't have to do any garbage collecting at this level 
        queue->head = curr->next;
    else
        last->next = curr->next;
    queue->size--;
    return 0;
}

int mq_is_empty(MinQueue *queue)
{
    return queue->size == 0;
}
