#include <efi.h>
#include <efilib.h>

#include "efi_stage.h"
#include "graphics.h"

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

EFI_STATUS efi_stage_get_mem_map(EFI_SYSTEM_TABLE *SysTable, EFI_MEMORY_DESCRIPTOR *mem_map, UINTN *map_size, UINTN *desc_size, UINTN *map_key) {
    EFI_STATUS status;
    UINTN alloc_pages = 0;
    UINT32 desc_ver = 0;
    EFI_PHYSICAL_ADDRESS alloc_addr;

    //mem_map.Type = EfiLoaderData;
    //mem_map.Attribute = EFI_MEMORY_RUNTIME;

    Print(L"Getting memory map...\n");

    // Get size of memory map first, then allocate number of pages necessary before calling GetMemoryMap() again. If still not enough, reallocate and try again
    while ((status = uefi_call_wrapper(SysTable->BootServices->GetMemoryMap, 5, map_size, mem_map, map_key, desc_size, &desc_ver)) != EFI_SUCCESS) {
        if (status == EFI_INVALID_PARAMETER)
            Print(L"Invalid Parameter.\n");
        else if (status == EFI_BUFFER_TOO_SMALL)
            Print(L"Buffer is too small.\n");

        if (alloc_pages) { // If we have allocated pages for the memory map already but buffer is still too small, deallocate them before reallocating
            Print(L"Buffer too small. Attempting to free allocated pages...\n");

            status = uefi_call_wrapper(SysTable->BootServices->FreePages, 2, alloc_addr, alloc_pages);

            if (status == EFI_SUCCESS)
                Print(L"Freed %u page(s) at start address %x\n", alloc_pages, alloc_addr);
            else {
                if (status == EFI_NOT_FOUND)
                    Print(L"The requested memory pages were not allocated.\n");
                else if (status == EFI_INVALID_PARAMETER)
                    Print(L"Memory is not a page-aligned address or Pages is invalid.\n");
                WaitForSingleEvent(SysTable->ConIn->WaitForKey, 0);
                return status;
            }
        }

        alloc_pages = *map_size / 4096 + 1; // allocate the number of pages (4K) needed for memory map
        status = uefi_call_wrapper(SysTable->BootServices->AllocatePages, 4, AllocateAnyPages, mem_map->Type, alloc_pages, &alloc_addr);
        mem_map = (EFI_MEMORY_DESCRIPTOR*) alloc_addr;
        *map_size = alloc_pages * 4096;

        if (status == EFI_SUCCESS) {
            Print(L"Allocated %u page(s) at start address %x\n", alloc_pages, alloc_addr);
        } else {
            if (status == EFI_OUT_OF_RESOURCES)
                Print(L"The pages could not be allocated.\n");
            else if (status == EFI_NOT_FOUND)
                Print(L"The requested pages could not be found.\n");
            else if (status == EFI_INVALID_PARAMETER)
                Print(L"Memory is NULL.\n");
            WaitForSingleEvent(SysTable->ConIn->WaitForKey, 0);
            return status;
        }
    }
    return status;
}
