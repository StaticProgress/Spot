#include <efi.h>
#include <efilib.h>

void kprint(UINTN *frame_buffer, UINTN line, UINTN column, UINT32 color, char *string);
void kprint_m(UINTN *frame_buffer, UINT32 color, char *string);

