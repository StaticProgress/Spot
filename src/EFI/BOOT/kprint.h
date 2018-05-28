#include <efi.h>
#include <efilib.h>

void kprint(UINT8 *frame_buffer, UINTN line, UINTN column, UINT32 color, char *string);