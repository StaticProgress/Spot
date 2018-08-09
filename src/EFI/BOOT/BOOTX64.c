#include <efi.h>
#include <efilib.h>

#include "graphics.h"
#include "kprint.h"
#include "print.h"
#include "string.h"
#include "efi_stage.h"
#include "kmain.h"

VIDEO_DEVICE video_output = {0};

EFI_STATUS
efi_main(EFI_HANDLE Image, EFI_SYSTEM_TABLE *SysTable) {
	InitializeLib(Image, SysTable);

    EFI_STATUS status;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
	UINTN handle_count = 0;
	EFI_HANDLE *handle_buffer;

    status = efi_stage_print_debug_addr(&Image, SysTable);
    if(status != EFI_SUCCESS)
        Print(L"Error Trying to get Loaded Image Base Address!\n");

    // -- SETTING UP GRAPHICS HANDLE --
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
	video_output.frame_buffer_base = (UINT8 *)gop->Mode->FrameBufferBase;
	UINTN info_size = 0, desired_mode = 0;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info[video_output.max_mode];
	VIDEO_MODE *modes[video_output.max_mode];
	status = uefi_call_wrapper(SysTable->BootServices->AllocatePool, 3, EfiRuntimeServicesData, sizeof(VIDEO_MODE) * video_output.max_mode, (VOID **)&modes);
	video_output.all_modes = (VIDEO_MODE **)modes;

	Print(L"Querying %u modes for highest supported resolution...\n", video_output.max_mode);

	for (UINT32 mode = 0, res = 0; mode < video_output.max_mode; mode++) {
		info[mode] = NULL;
		status = gop->QueryMode(gop, mode, &info_size, (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **)&info[mode]);
		if (info[mode]->PixelFormat < PixelBltOnly) {
			modes[mode]->mode_id = mode;
			modes[mode]->h_res = info[mode]->HorizontalResolution;
			modes[mode]->v_res = info[mode]->VerticalResolution;
			modes[mode]->pixel_per_scan_line = info[mode]->PixelsPerScanLine;
			modes[mode]->draw_pixel = select_draw_pixel(&info[mode]->PixelInformation, info[mode]->PixelFormat);
			if (modes[mode]->h_res * modes[mode]->v_res > res) {
				desired_mode = mode;
				res = modes[mode]->h_res * modes[mode]->v_res;
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

	Print(L"%ux%u\n", video_output.all_modes[video_output.cur_mode]->h_res, video_output.all_modes[video_output.cur_mode]->v_res);


    //-- GETTING MEMORY MAP --
    EFI_MEMORY_DESCRIPTOR *mem_map = NULL;
    UINTN map_size = 0, map_key = 0, desc_size = 0;
	status = efi_stage_get_mem_map(SysTable, mem_map, &map_size, &desc_size, &map_key);

    // -- DO NOT COMPARTMENTALIZE THESE PARTS --

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

    kmain(mem_map, map_size, desc_size);

	return status;
}
