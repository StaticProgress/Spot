#ifndef MEMALLOC_H
#define MEMALLOC_H

#include <efi.h>

void init_malloc(void *free_start, UINTN size);
void *malloc(UINTN size);
void free(void *ptr);

#endif
