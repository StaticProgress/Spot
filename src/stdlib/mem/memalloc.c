#include <efi.h>
#include "memalloc.h"
#include "ppm.h"

//TODO: Map the pages to virtual memory after receiving them from the ppm.

struct MEM_Node {
    UINTN size;
    struct MEM_Node *next;
};

struct MEM_Node *free_blocks = NULL;

void init_malloc(void *free_start, UINTN size)
{
    free_blocks = free_start;
    free_blocks->size = size - sizeof(struct MEM_Node);
}

//Easy allocate, shrink the current free node down.
//Allocate a node behind the allocated area.
static void *split_alloc(struct MEM_Node *node, UINTN size) {
    node->size -= size + sizeof(struct MEM_Node);
    //Allocate a node behind the allocated area.
    struct MEM_Node *alloc_node = node + sizeof(struct MEM_Node) + node->size;
    alloc_node->size = size;
    alloc_node->next = node->next;
    node->next = alloc_node->next;
    return alloc_node + sizeof(struct MEM_Node);
}

//The current allocation is the same size of the free block (or about right),
//so we delete the current free block.
static void *delete_alloc(struct MEM_Node *last_node, struct MEM_Node *curr_node) {
    if(!last_node)
        last_node->next = curr_node->next;
    else
        free_blocks = curr_node->next;
    return curr_node + sizeof(struct MEM_Node);
}

static void *malloc_small(UINTN size) {
    //Find a node of the right size, or return null if not found.
    struct MEM_Node *last_node = NULL;
    for(struct MEM_Node *curr_node = free_blocks; curr_node != NULL; curr_node = curr_node->next) {
        if(curr_node->size > (sizeof(struct MEM_Node) + size)) {
            //Large fit
            return split_alloc(curr_node, size);
        } else {
            //Perfect fit
            return delete_alloc(last_node, curr_node);
        }
    }
    return NULL;
}

void *malloc(UINTN size)
{
    //If the node is larger than a page malloc, return a new multiple page alloc.
    if(size > PAGE_SIZE - sizeof(struct MEM_Node) && pages_free()) {
        struct MEM_Node *node = get_pages((size + sizeof(struct MEM_Node)) / PAGE_SIZE + 1);
        node->size = size + sizeof(struct MEM_Node) + PAGE_SIZE;
        free(node);
        return node;
    }

    void *addr = malloc_small(size);
    if(addr)
        return addr;

    //If there is no node of the right size then get a new page.
    if(pages_free()) {
        struct MEM_Node *new_node = get_pages(1);
        new_node->size = PAGE_SIZE - sizeof(new_node);
        free(new_node);
        return malloc_small(size);
    }
    return NULL;
}


void free(void *p)
{
    //TODO: This could be wayyyy more efficient.
    /* For example, we could use a minQueue so we don't have to search the whole
     * list for matches each time.
     * We also should not be doing a front insertion with every free, but for
     * now it will have to work...
     */

    //First cast this to the right type of pointer
    struct MEM_Node *ptr = (struct MEM_Node*)(p);

    //If we get a null ptr then this malloc failed, just quit
    if(!ptr)
        return;
    //Else shift the ptr back to what it used to be.
    ptr -= sizeof(struct MEM_Node);

    //Make sure that it can't be combined with other free blocks.
    for(struct MEM_Node *curr_node = free_blocks; curr_node != NULL; curr_node = curr_node->next) {
        if(curr_node + sizeof(struct MEM_Node) + curr_node->size == ptr) {
            curr_node->size += ptr->size + sizeof(struct MEM_Node);
            return;
        }
    }

    //Else just add it back the free_blocks list
    ptr->next = free_blocks;
    free_blocks = ptr;
}
