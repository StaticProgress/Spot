#include <stdint.h>
#include <efi.h>
#include <efilib.h>

EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *ST) {
	InitializeLib(image, ST);

	EFI_STATUS status;

	UINTN map_size = 0, map_key = 0, desc_size = 0, alloc_pages = 0;
	UINT32 desc_ver = 0;

	EFI_PHYSICAL_ADDRESS alloc_addr;

	EFI_MEMORY_DESCRIPTOR mem_map;
	mem_map.Type = EfiLoaderData;
	mem_map.Attribute = EFI_MEMORY_RUNTIME;

	Print(L"Getting memory map\n");

	while ((status = uefi_call_wrapper(ST->BootServices->GetMemoryMap, 5, &map_size, &mem_map, &map_key, &desc_size, &desc_ver)) == EFI_BUFFER_TOO_SMALL) {
		if (alloc_pages) {
			status = uefi_call_wrapper(ST->BootServices->FreePages, 2, alloc_addr, alloc_pages);

			if (status == EFI_SUCCESS)
				Print(L"Freed %u page(s) at start address %x\n", alloc_pages, alloc_addr);
			else {
				if (status == EFI_NOT_FOUND)
					Print(L"The requested memory pages were not allocated.\n");
				else if (status == EFI_INVALID_PARAMETER)
					Print(L"Memory is not a page-aligned address or Pages is invalid.\n");
				WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
				return status;
			}
		}

		alloc_pages = map_size / 4096 + 1;
		status = uefi_call_wrapper(ST->BootServices->AllocatePages, 4, AllocateAnyPages, mem_map.Type, alloc_pages, &alloc_addr);
		mem_map.PhysicalStart = alloc_addr;
		map_size = alloc_pages * 4096;

		if (status == EFI_SUCCESS)
			Print(L"Allocated %u page(s) at start address %x\n", alloc_pages, alloc_addr);
		else {
			if (status == EFI_OUT_OF_RESOURCES)
				Print(L"The pages could not be allocated.\n");
			else if (status == EFI_NOT_FOUND)
				Print(L"The requested pages could not be found.\n");
			else if (status == EFI_INVALID_PARAMETER)
				Print(L"Memory is NULL.\n");
			WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
			return status;
		}
	}

	if (status == EFI_INVALID_PARAMETER) {
		Print(L"Failed to get memory map");
		WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
		return status;
	}

	// cannot use any function between getting the memory map and exiting boot services since it will invalidate the map key
	status = uefi_call_wrapper(ST->BootServices->ExitBootServices, 2, image, map_key);

	if (status != EFI_SUCCESS) {
		Print(L"Could not exit, invalid memory key\n");
		WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
		return status;
	}

	return EFI_SUCCESS;
}
