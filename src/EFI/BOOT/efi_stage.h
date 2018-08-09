#ifndef EFI_STAGE_H
#define EFI_STAGE_H

/* These functions are for use in the main efi boot method.
 * Because they use EFI functions they are not to be used anywhere else in the
 * project.
 */

#include <efi.h>

EFI_STATUS efi_stage_print_debug_addr(EFI_HANDLE *Image, EFI_SYSTEM_TABLE *SysTable);
EFI_STATUS efi_stage_get_mem_map(EFI_SYSTEM_TABLE *SysTable, EFI_MEMORY_DESCRIPTOR *mem_map, UINTN *map_size, UINTN *desc_size, UINTN *map_key);

#endif
