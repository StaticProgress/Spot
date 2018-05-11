#include <stdint.h>
#include <efi.h>
#include <efilib.h>

#define MEM_MAP_PAGE_NUM 2

/* Args:
 *  mem_map_start = The address to store the allocated pages for the memory map.
 */
void allocate_mem_map(EFI_PHYSICAL_ADDRESS *mem_map_start)
{
    uint32_t status = uefi_call_wrapper(ST->BootServices->AllocatePages, 4, AllocateAnyPages, EfiLoaderData, MEM_MAP_PAGE_NUM, mem_map_start);
    if(status != EFI_SUCCESS) {
        Print(L"Error: Not able to allocate pages for memory map.\n");
    }
}

void get_memory_map(EFI_MEMORY_DESCRIPTOR *mem_map, uint64_t *mem_map_size, uint64_t *map_key)
{
    EFI_PHYSICAL_ADDRESS mem_map_start;
    allocate_mem_map(&mem_map_start);

    mem_map->Type = EfiLoaderData,
    mem_map->PhysicalStart = mem_map_start,
    mem_map->VirtualStart = 0,
    mem_map->NumberOfPages = MEM_MAP_PAGE_NUM,
    mem_map->Attribute = EFI_MEMORY_RUNTIME;
    
    *mem_map_size = MEM_MAP_PAGE_NUM * 1024;
    uint64_t desc_size; 
    uint32_t desc_version;
    uint32_t status = uefi_call_wrapper(ST->BootServices->GetMemoryMap, 5, mem_map_size, mem_map, map_key, &desc_size, &desc_version);
    
    if(status != EFI_SUCCESS) {
        Print(L"Error: Not able to get memory map");
    }
}

EFI_STATUS efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
	InitializeLib(image, systab);
    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
	Print(L"SpotOS!\n");

    //Get a memory map
    EFI_MEMORY_DESCRIPTOR mem_map;
    uint64_t map_key; //Used to tell if the memory map is up to date.
    uint64_t mem_map_size; //Size in bytes of the memory map array.
    get_memory_map(&mem_map, &mem_map_size, &map_key);
   
    uint32_t status = uefi_call_wrapper(ST->BootServices->ExitBootServices, 2, image, map_key);
    if(status != EFI_SUCCESS) {
        Print(L"Error: Not able to exit boot services");
    }

    //Writing a little bit to Vga memory
    //Good reading: http://www.osdever.net/FreeVGA/vga/vgamem.htm
    char *vga_mem = (char*) 0x000b8000;
    *vga_mem = 'T'; 

    return EFI_SUCCESS;
}
