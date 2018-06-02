#include "string.h"
#include <efi.h>

void *memcpy(void *dest, void *src, UINTN size) {
    //Copy byte by byte
    for(UINTN i = (UINTN)src; i < (UINTN)(src + size); i++) {
        *(char*)(dest + i) = *(char*)(src + i);
    }
    return dest;
}

void *memset(void *src, int byte, UINTN size) {
    for(UINTN i = (UINTN)src; i < (UINTN)(src + size); i++)
        *(char*)(src + i) = (char) byte;
    return src;
}
