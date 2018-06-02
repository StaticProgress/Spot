#ifndef STRING_H
#define STRING_H

#include <efi.h>

//Copies non-overlaping memory regions from src to dest.
void *memcpy(void *dest, void *src, UINTN size);
//Fills the memory with the specified byte.
void *memset(void *src, int byte, UINTN size);

#endif
