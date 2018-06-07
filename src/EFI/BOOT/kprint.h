#ifndef KPRINT_H
#define KPRINT_H

#include <efi.h>
#include <efilib.h>

void kprint_char(UINTN x, UINTN y, UINT32 color, char character);
void kprint(UINTN line, UINTN column, UINT32 color, char *string);
void kscroll(UINTN rows);

#endif
