#include <efi.h>

#include "ppm.h"
#include "memalloc.h"

struct PPM_Node {
    char *addr;
    UINTN page_num;
    struct PPM_Node *next;
    struct PPM_Node *last;
};

struct PPM_Node *free_pages = NULL;

int pages_free() {
    return free_pages != NULL;
}

static int is_free_mem(EFI_MEMORY_TYPE type) {
    switch (type) {
        case EfiBootServicesCode:
        case EfiBootServicesData:
        case EfiConventionalMemory:
            return 1;
        default:
            return 0;
    }
}

/*
 * Because we insert all these PPM_Nodes into the list with front insertion,
 * and because the efi_mem_map is in physical address order, we only have to
 * check one node to see if its continguous.
 */
static int continguous_pages(EFI_MEMORY_DESCRIPTOR *desc) {
    if(pages_free() && (UINTN)(free_pages->addr + (free_pages->page_num * PAGE_SIZE)) == desc->PhysicalStart) {
        //Back-to-back pages
        free_pages->page_num += desc->NumberOfPages;
        return 1;
    }
    return 0;
}

/*
 * Finishes converting the efi_memory_map into being managed by the ppm,
 * but uses the newly available malloc call.
 */
static void convert_efi_map(EFI_MEMORY_DESCRIPTOR *efi_mem_map, UINTN efi_map_size, UINTN desc_size) {
    for(UINTN offset = 0; offset < efi_map_size; offset += desc_size) {
        EFI_MEMORY_DESCRIPTOR *curr_desc = (EFI_MEMORY_DESCRIPTOR*)((char*)(efi_mem_map) + offset);
        if(is_free_mem(curr_desc->Type)) {
            //Then add it to the linked list, this is a costly O(n^2) first run as
            //we have to combine all the back-to-back pages.
            if(!continguous_pages(curr_desc)) {
                //If we cannot add this to an existing PPM_Node, then make a new
                //node for these pages (Simple front insertion).
                struct PPM_Node *node = malloc(sizeof(struct PPM_Node));
                node->addr = (char*) curr_desc->PhysicalStart;
                node->page_num = curr_desc->NumberOfPages;
                node->next = free_pages;
                node->last = NULL;
                free_pages = node;
            }
        }
    }
}

/*
 * Grab the first couple free pages and use them to setup a basic malloc that
 * will be used to keep track of the rest of the pages.
 */
void setup_ppm(EFI_MEMORY_DESCRIPTOR *efi_mem_map, UINTN efi_map_size, UINTN desc_size) {
    #define INITIAL_PAGES 2
    //Setup a page for the normal allocater so that we can get malloc working
    //as quickly as humanly possible.

    //First get the first two free pages.
    for(UINTN offset = 0; offset < efi_map_size; offset += desc_size) {
        EFI_MEMORY_DESCRIPTOR *curr_desc = (EFI_MEMORY_DESCRIPTOR*)((char*)(efi_mem_map) + offset);
        if(is_free_mem(curr_desc->Type)) {
            if(curr_desc->NumberOfPages > INITIAL_PAGES) {
                free_pages = (struct PPM_Node*) curr_desc->PhysicalStart;
                curr_desc->PhysicalStart += PAGE_SIZE * INITIAL_PAGES;
                curr_desc->NumberOfPages -= INITIAL_PAGES;
            }
        }
    }

    //Now use the first page as the starting point of the free_pages list, and
    //the second page as the first free page.
    free_pages->addr = (char*)(free_pages) + PAGE_SIZE;
    free_pages->page_num = INITIAL_PAGES - 1;
    free_pages->next = NULL;
    free_pages->last = NULL;

    init_malloc(free_pages->addr + sizeof(struct PPM_Node),
                PAGE_SIZE - sizeof(struct PPM_Node));

    convert_efi_map(efi_mem_map, efi_map_size, desc_size);
}

/* Get pages returns the address to page_num continguous pages.
 * It will return NULL if pages are not available.
 */
 void *get_pages(UINTN page_num) {
    //For now just traverse free_pages until we find enough free pages.
    for(struct PPM_Node *curr = free_pages; curr != NULL; curr = curr->next) {
        if(curr->page_num > page_num) {
            curr->page_num -= page_num;
            void *addr = curr->addr;
            curr->addr += page_num * PAGE_SIZE;
            return addr;
        } else if(curr->page_num == page_num) {
            //We need to de-allocate this node.
            void *addr = curr->addr;
            if(curr->last != NULL)
                curr->last->next = curr->next;
            else
                free_pages = curr->next;
            return addr;
        }
    }
    return NULL;
}
