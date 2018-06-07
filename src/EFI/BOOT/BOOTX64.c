#include <efi.h>
#include <efilib.h>

#include "boot.h"
#include "graphics.h"
#include "kprint.h"
#include "print.h"
#include "string.h"

VIDEO_DEVICE video_output = {0};

EFI_STATUS
efi_main(EFI_HANDLE Image, EFI_SYSTEM_TABLE *SysTable) {
	InitializeLib(Image, SysTable);
    EFI_STATUS status;

    setup_gop(Image); 
    
	UINTN map_size = 0, map_key = 0, desc_size = 0, alloc_pages = 0;
	UINT32 desc_ver = 0;

	EFI_PHYSICAL_ADDRESS alloc_addr;

	EFI_MEMORY_DESCRIPTOR mem_map;
	mem_map.Type = EfiLoaderData;
	mem_map.Attribute = EFI_MEMORY_RUNTIME;

	Print(L"Getting memory map...\n");

	// Get size of memory map first, then allocate number of pages necessary before calling GetMemoryMap() again. If still not enough, reallocate and try again
	while ((status = uefi_call_wrapper(SysTable->BootServices->GetMemoryMap, 5, &map_size, &mem_map, &map_key, &desc_size, &desc_ver)) == EFI_BUFFER_TOO_SMALL) {
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

		Print(L"%u bytes required for memory map. Allocating pages...\n");

		alloc_pages = map_size / 4096 + 1; // allocate the number of pages (4K) needed for memory map
		status = uefi_call_wrapper(SysTable->BootServices->AllocatePages, 4, AllocateAnyPages, mem_map.Type, alloc_pages, &alloc_addr);
		mem_map.PhysicalStart = alloc_addr;
		map_size = alloc_pages * 4096;

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

	if (status == EFI_INVALID_PARAMETER) { // check if last GetMemoryMap was successful
		Print(L"Failed to get memory map");
		WaitForSingleEvent(SysTable->ConIn->WaitForKey, 0);
		return status;
	}

	// ! cannot use any function between getting the memory map and exiting boot services since it will invalidate the map key !
	status = uefi_call_wrapper(SysTable->BootServices->ExitBootServices, 2, Image, map_key);

	if (status != EFI_SUCCESS) {
		Print(L"Could not exit boot services, invalid memory key\n");
		WaitForSingleEvent(SysTable->ConIn->WaitForKey, 0);
		return status;
	}

	clear_screen();

	// the old method is probably way more efficient so I'm leaving it here
	//for (UINT32 i = 0, *p = (UINT32 *)gop->Mode->FrameBufferBase; i < (gop->Mode->FrameBufferSize / 4); i++, p++) *p = 0;

	// get frame buffer base and draw a square. note that qemu's 1024x768 mode is 24 bits (found in PixelBitMask)
	UINTN x = 1024 / 2, y = 768 / 2, w = 100, h = 50;
    draw_rect(x - (w / 2), y - (h / 2), w, h, 0xFFFFFF);

	// function to print string at specified x and y coordinates
    kprint(x - 45, y - 14, 0xEF7223, "Welcome to\nSpot OS!\n");

	return status;
}
