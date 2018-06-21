#include <efi.h>
#include "memalloc.h"
#include "minqueue.h"

#define INITIAL_MEM_SIZE 0xFFFFFFFFFFFFFFFF

//TODO: Actually allocate invidual pages to a program and then use that until it is full.
/* Super simple memory allocater that just looks for the first hole that fits the requested size.
 * Then it cuts the excess off of that hole and creates a hole farther down.
 */

//NonQueue hole at begginning size
static UINTN SIZE;

//Queues to keep track of the allocated chunks and holes in between.
static MinQueue allocated;
static MinQueue holes;

//For last minute validity checks.
static void *HEAP_START;
static void *HEAP_END;

void init_malloc(void *holes_start, UINTN init_size)
{
    HEAP_START = holes_start;
    HEAP_END = holes_start + init_size;
    SIZE = init_size;

    allocated.head = NULL;
    allocated.size = 0;
    //Initial size of the first hole should be the addr where it starts to the end of memory.
    holes.head = NULL;
    holes.size = 0;
}

//Initializes the memory allocater based on a memory map from UEFI boot services.
//This way we keep the originally allocated things like our program without messing them up!
void init_malloc_efi(EFI_MEMORY_DESCRIPTOR *mem_map, UINTN descriptor_size, UINTN map_size) {
    //Start the initial hole in low memory around 4MB (0x400000)
    //And allow the allocater to place anywhere above that.
    init_malloc((void*) 0x400000, INITIAL_MEM_SIZE - 0x400000);
    for(UINTN i = (mem_map + map_size) - descriptor_size; i += descriptor_size) {
        EFI_MEMORY_DESCRIPTOR *current_alloc = mem_map + i;
        //Only want to keep memory that is necessary, we can throw away efi boot memory.
        if(current_alloc->Type != EfiBootServicesCode || current_alloc->type != EfiBootServicesData) {
            //TODO: Use virtual addresses when we get to that point instead of physical addressing.
            //Make sure we can replace the header for this allocation with our MinQueue Node header.
            if(current_alloc->) 
        }
    }
}

//Returns the addr of the highest node or HEAP_END if there is no node higher / allocated yet.
static void *get_highest_addr() {
        if(allocated.head == NULL && holes.head == NULL)
            return HEAP_END;
        //If holes is null or allocated's head has the highest addr then it is the highest addr.
        else if(holes.head == NULL || (allocated.head != NULL && holes.head != NULL && &(allocated.head) > &(holes.head)))
            return allocated.head;
        else
            return holes.head;
}

//Allocates a new node in the begginning hole.
//Returns NULL on error or a valid pointer to a QueueNode's data on success.
static void *non_queue_alloc(uint64_t size) {
    //The reason we check both size and sizeof is because we want to prevent overflow.
    if(size < SIZE && (size + sizeof(QueueNode)) <= SIZE) {
        //We have enough space to make a hole.
        void *highest_addr = get_highest_addr();
        //Allocate node at highest addr.
        void *node_addr = highest_addr - size - sizeof(QueueNode);
        mq_add(&allocated, node_addr, size);
        SIZE -= size + sizeof(QueueNode);
        return node_addr;
    }
    return NULL;
}

void *malloc(UINTN size)
{
    //Look for hole that is big enough.
    QueueNode *curr_hole;
    for(curr_hole = holes.head; curr_hole != NULL && curr_hole->data < size; curr_hole = curr_hole->next);
    if(!curr_hole) {
        //If the current hole is null we should try to allocate a new hole in the left over space. By going to the smallest address
        //and subtracting the size of the hole.
        void* addr = non_queue_alloc(size);
        return (addr == NULL)? addr : addr + sizeof(QueueNode);
    }
    //Now we need to check if we need to make another hole.
    //(Size of this allocated block) - (Header size for next hole)
    uint64_t next_hole_size = (curr_hole->data - size) - sizeof(QueueNode);
    if(next_hole_size > 0) {
        //This means that we should plan to create a new hole.
        void* addr_of_next_hole = (void*)(curr_hole) + sizeof(QueueNode) + size;
        mq_add(&holes, addr_of_next_hole, next_hole_size);
    }
    //We should swap this hole node into the allocated list.
    //We need to 'remove' it from the hole node list, this WILL NOT modify any data within this node.
    mq_remove(&holes, curr_hole);
    //Then we need to use the original address on the heap.
    mq_addn(&allocated, curr_hole);
    //Check for last minute validity
    if((void*)curr_hole > HEAP_END || (void*)curr_hole < HEAP_START)
        return NULL;
    //Then finally return the pointer.
    return (void*)(curr_hole) + sizeof(QueueNode);
}


void free(void *ptr)
{
    //If we get a null ptr then this malloc failed, just quit
    if(!ptr)
        return;
    //Else shift the ptr back to what it used to be.
    ptr -= sizeof(QueueNode);
    //Remove from the allocated list
    mq_remove(&allocated, (QueueNode*)ptr);
    //Add the pointer to the hole list.
    mq_addn(&holes, (QueueNode*) ptr);
    //Check if the following hole can be combined with this one repeatedly.
    QueueNode *curr = ptr;
    while(curr->next != NULL && (curr + curr->data) == curr->next) {
        curr->data += curr->next->data;
        mq_remove(&holes, curr->next);
    }
    //Check to see if highest_addr is the holes.head, if so we need to add it back to the
    //beginning hole.
    void *highest_addr = get_highest_addr();
    if(highest_addr == holes.head) {
        //Add this node back into the main hole.
        SIZE += sizeof(QueueNode) + holes.head->data;
        mq_remove(&holes, holes.head);
    }
}
