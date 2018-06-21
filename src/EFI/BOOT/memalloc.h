#ifndef MEMALLOC_H
#define MEMALLOC_H

#include <efi.h>

void init_malloc(void* holes_start, UINTN size);
void init_malloc_efi(EFI_MEMORY_DESCRITOR *mem_map, UINTN descriptor_size, UINTN map_size);
void *malloc(UINTN size);
void free(void *ptr);

#endif
