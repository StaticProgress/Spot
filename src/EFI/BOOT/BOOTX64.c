#include <efi.h>
#include <efilib.h>

#include "graphics.h"
#include "kprint.h"

VIDEO_DEVICE video_output = {0};

EFI_STATUS
efi_main(EFI_HANDLE Image, EFI_SYSTEM_TABLE *SysTable) {
	InitializeLib(Image, SysTable);

	EFI_STATUS status;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
	UINTN handle_count = 0;
	EFI_HANDLE *handle_buffer;

	Print(L"Locating handles that support the Graphics Output Protocol...\n");

	status = uefi_call_wrapper(SysTable->BootServices->LocateHandleBuffer, 3, ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL, &handle_count, &handle_buffer);

	if (status == EFI_SUCCESS)
		Print(L"%u handle(s) were successfully returned.\n", handle_count);
	else {
		if (status == EFI_NOT_FOUND)
			Print(L"No handles match the search.\n");
		else if (status == EFI_BUFFER_TOO_SMALL)
			Print(L"The BufferSize is too small for the result. BufferSize has been updated with the size needed to complete the request.\n");
		else if (status == EFI_INVALID_PARAMETER)
			Print(L"Invalid parameter.\n");
		WaitForSingleEvent(SysTable->ConIn->WaitForKey, 0);
		return status;
	}

	Print(L"Attempting to open handle interface to GOP...\n");

	status = uefi_call_wrapper(SysTable->BootServices->OpenProtocol, 6, handle_buffer[0], &gEfiGraphicsOutputProtocolGuid, (VOID **)&gop, Image, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

	if (status == EFI_SUCCESS)
		Print(L"Success. Found %u supported modes.\n", gop->Mode->MaxMode);
	else {
		if (status == EFI_UNSUPPORTED)
			Print(L"Handle does not support GOP.\n");
		else if (status == EFI_INVALID_PARAMETER)
			Print(L"Invalid parameter.\n");
		else if (status == EFI_ACCESS_DENIED)
			Print(L"Access denied.\n");
		else if (status == EFI_ALREADY_STARTED)
			Print(L"Already opened.\n");
		WaitForSingleEvent(SysTable->ConIn->WaitForKey, 0);
		return status;
	}

	video_output.max_mode = gop->Mode->MaxMode;
	video_output.cur_mode = gop->Mode->Mode;
	UINTN info_size = 0, desired_mode = 0;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info[video_output.max_mode];
	VIDEO_MODE modes[video_output.max_mode];
	video_output.all_modes = (VIDEO_MODE **)&modes;

	Print(L"Querying %u modes for highest supported resolution...\n", video_output.max_mode);

	for (UINT32 mode = 0, res = 0; mode < video_output.max_mode; mode++) {
		info[mode] = NULL;
		status = gop->QueryMode(gop, mode, &info_size, (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **)&info[mode]);
		if (info[mode]->PixelFormat < PixelBltOnly) {
			modes[mode].mode_id = mode;
			modes[mode].h_res = info[mode]->HorizontalResolution;
			modes[mode].v_res = info[mode]->VerticalResolution;
			modes[mode].pixel_per_scan_line = info[mode]->PixelsPerScanLine;
			modes[mode].draw_pixel = select_draw_pixel(&info[mode]->PixelInformation);
			if (modes[mode].h_res * modes[mode].v_res > res) {
				desired_mode = mode;
				res = modes[mode].h_res * modes[mode].v_res;
			}
		}
	}

	Print(L"Press any key to change resolution.\n");

	WaitForSingleEvent(SysTable->ConIn->WaitForKey, 0);
	status = gop->SetMode(gop, desired_mode);

	if (status == EFI_SUCCESS) {
		video_output.cur_mode = desired_mode;
		Print(L"Switched to ");
	} else
		Print(L"Failed to set mode. Current mode is ");

	Print(L"%ux%u\n", gop->Mode->Info->HorizontalResolution, gop->Mode->Info->VerticalResolution);

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

	/* 	for (UINT32 i = 0, *p = (UINT32 *)gop->Mode->FrameBufferBase; i < (gop->Mode->FrameBufferSize / 4); i++, p++) *p = 0;

	    // get frame buffer base and draw a square. note that qemu's 1024x768 mode is 24 bits (found in PixelBitMask)
	    UINTN x = 1024 / 2, y = 768 / 2, w = 100, h = 50;
	    UINT8 red = 0xFF, green = 0xFF, blue = 0xFF;
	    UINT8 *frame_base = (UINT8 *)gop->Mode->FrameBufferBase;

	    // draw square in center of screen
	    for (UINT8 r = 0, *frame_ptr = frame_base + (1024 * (y - h / 2) + x - w / 2) * 3; r < h; r++) {
	        for (UINTN c = 0; c < w; c++) {
	            *frame_ptr++ = blue;
	            *frame_ptr++ = green;
	            *frame_ptr++ = red;
	        }
	        frame_ptr += (1024 - w) * 3;
	    }

	    // function to print string at specified x and y coordinates
	    kprint(frame_base, x - 45, y - 14, 0xEF7223, "Welcome to\nSpot OS!\n"); */

	return status;
}
