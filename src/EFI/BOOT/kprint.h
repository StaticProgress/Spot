#include <efi.h>
#include <efilib.h>
#ifndef KPRINT_H
#define KPRINT_H

#include <efi.h>

void kprint(UINT8 *frame_buffer, UINTN line, UINTN column, UINT32 color, char *string);
void kprint_m(UINT8 *frame_buffer, UINT32 color, char *string);
void kscroll(UINT8 *frame_buffer_base, UINTN rows);

#endif
