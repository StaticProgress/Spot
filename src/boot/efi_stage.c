#include <efi.h>
#include <efilib.h>

#include "efi_stage.h"
#include "stdlib/graphics.h"

EFI_STATUS efi_stage_print_debug_addr(EFI_HANDLE *Image, EFI_SYSTEM_TABLE *SysTable) {
    EFI_STATUS status;
    EFI_LOADED_IMAGE *loaded_image = NULL;
    status = uefi_call_wrapper(SysTable->BootServices->HandleProtocol, 3, *Image, &gEfiLoadedImageProtocolGuid, (void**) &loaded_image);

    if(status != EFI_SUCCESS)
        return status;

    Print(L"Make sure Image Base is located at the right base...\n");
    Print(L"Image Base: 0x%08lx\n", loaded_image->ImageBase);
    return status;
}

EFI_STATUS efi_stage_get_mem_map(EFI_SYSTEM_TABLE *SysTable, EFI_MEMORY_DESCRIPTOR **mem_map, UINTN *map_size, UINTN *desc_size, UINTN *map_key) {
    EFI_STATUS status;
    UINT32 *desc_ver = NULL;

    Print(L"Getting memory map...\n");

    // Get size of memory map first, then allocate number of pages necessary before calling GetMemoryMap() again.
    status = uefi_call_wrapper(SysTable->BootServices->GetMemoryMap, 5, map_size, *mem_map, map_key, desc_size, desc_ver);

    // Increase the size of the memory map by a bit because we are going to increase it when allocating pages for it.

    *map_size += *desc_size * 2;
    status = uefi_call_wrapper(SysTable->BootServices->AllocatePool, 3, EfiLoaderData, *map_size, (void**) mem_map);

    if(status != EFI_SUCCESS) {
        Print(L"Problem allocating memory for map!\n");
        return status;
    }

    //Call getMemMap again but this time with the right sized everything.
    status = uefi_call_wrapper(SysTable->BootServices->GetMemoryMap, 5, map_size, *mem_map, map_key, desc_size, desc_ver);

    if(status != EFI_SUCCESS) {
        Print(L"Problem getting memory map!\n");
        return status;
    }

    return status;
}
