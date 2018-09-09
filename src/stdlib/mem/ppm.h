#ifndef PPM_H
#define PPM_H

#include <efi.h>

/*
 * Physical Page Manager:
 * Allocates pages on the physical address level, then maps them to a virtual
 * address.
 */

 #define PAGE_SIZE 4096

/*
 * Setup PPM:
 * Calculates the amount of physical memory in the system, also
 * allocates memory for the kernel and its data.
 * Arguments:
 *      efi_mem_map: A pointer to the efi memory map.
 *      efi_map_size: The size of the efi memory map.
 *      desc_size: The size of each efi memory descriptor.
 */
void setup_ppm(EFI_MEMORY_DESCRIPTOR *efi_mem_map, UINTN efi_map_size, UINTN desc_size);
void *get_pages(UINTN page_num);
int pages_free();

#endif
