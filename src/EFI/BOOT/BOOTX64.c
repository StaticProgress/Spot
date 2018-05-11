#include <stdint.h>
#include <efi.h>
#include <efilib.h>

#define PAGE_NUM 4

void print_memory_map(EFI_MEMORY_DESCRIPTOR *start_addr, uint64_t map_size) {
    Print(L"Map Size: %lu\n", map_size);
    Print(L"Starts At / Ends At: %lu/%lu\n", start_addr, (void*)(start_addr) + map_size);
    EFI_MEMORY_DESCRIPTOR *addr;
    for(addr = start_addr; addr < (EFI_MEMORY_DESCRIPTOR*)((void*)(start_addr) + map_size); addr += 1) {
        Print(L"Start P/V: %lu/%lu\n", addr->PhysicalStart, addr->VirtualStart);
        Print(L"Page Num: %lu\n", addr->NumberOfPages);
        Print(L"Attrib: %lX\n", addr->Attribute);
    }
}

EFI_STATUS
efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
	InitializeLib(image, systab);
    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
	Print(L"We made it this far!\n");
    Print(L"Memory Map:\n");

    //Allocate a buffer for the memory map (4 Pages)
    EFI_PHYSICAL_ADDRESS mem_map_start;
    int32_t status = uefi_call_wrapper(ST->BootServices->AllocatePages, 4, AllocateAnyPages, EfiLoaderData, PAGE_NUM, &mem_map_start);
    if(status != EFI_SUCCESS) {
        Print(L"Failure to allocate pages!\n");
    }


    EFI_MEMORY_DESCRIPTOR mem_map = {
        .Type = EfiLoaderData,
        .PhysicalStart = mem_map_start,
        .VirtualStart = 0,
        .NumberOfPages = PAGE_NUM,
        .Attribute = EFI_MEMORY_RUNTIME
    };


    uint64_t mem_map_size = PAGE_NUM * 1024;
    uint64_t map_key;
    uint64_t desc_size;
    uint32_t desc_version;
    status = uefi_call_wrapper(ST->BootServices->GetMemoryMap, 5, &mem_map_size, &mem_map, &map_key, &desc_size, &desc_version);
   
    if(status != EFI_SUCCESS) {
        Print(L"Failed to allocate memory map!\n");
    } else {
        print_memory_map(&mem_map, mem_map_size);
    }

    Print(L"Memory Map Size in Pages: %lu\n", mem_map.NumberOfPages);
	Print(L"\n\n\nPress any key to exit\n");
	WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
	
    //DeAllocate pages
    uefi_call_wrapper(ST->BootServices->FreePages, 2, mem_map_start, PAGE_NUM);
    Print(L"Trying!\n");
    Print(L"Trying!\n");
    Print(L"Trying!\n");
    Print(L"Trying!\n");

    uefi_call_wrapper(ST->BootServices->Exit, 4, image, EFI_SUCCESS, 0, L"Done");
    return EFI_SUCCESS;
}
