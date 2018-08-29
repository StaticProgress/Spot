#ifndef STRING_H
#define STRING_H

#include <efi.h>

//Custom string header for SPOT OS. Until EFI.h goes away we have to use a custom namespace of spot_.

void *spot_memcpy(void *dest, void *src, UINTN size);
void *spot_memset(void *src, int constant, UINTN size);

#endif
