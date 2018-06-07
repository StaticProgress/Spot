#include <efi.h>
#include <efilib.h>

void setup_gop(EFI_HANDLE image) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    UINTN handle_count = 0;
    EFI_HANDLE *handle_buffer;
	Print(L"Locating handles that support the Graphics Output Protocol... ");
    handle_err(locate_gop(handle_count, handle_buffer));
    
	Print(L"Attempting to open handle interface to GOP... ");
    handle_err(open_gop(handle_buffer, gop, image));

    setup_video_output(gop);

	Print(L"Press any key to change resolution.\n");
	WaitForSingleEvent(SysTable->ConIn->WaitForKey, 0);
    handle_err(set_desired_mode(gop));	
}

EFI_STATUS locate_gop(handle_count, handle_buffer) {
	return uefi_call_wrapper(SysTable->BootServices->LocateHandleBuffer, 3, ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL, &handle_count, &handle_buffer);
}

EFI_STATUS open_gop(EFI_HANDLE *handle_buffer, EFI_GRAPHICS_OUTPUT_PROTOCOL *gop, EFI_HANDLE Image) {
	return uefi_call_wrapper(SysTable->BootServices->OpenProtocol, 6, handle_buffer[0], &gEfiGraphicsOutputProtocolGuid, (VOID **)&gop, Image, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
}

void setup_video_output(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop) {
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
                Print(L"Found a %dx%d resolution mode\n", modes[mode]->h_res, modes[mode]->v_res);
			}
		}
	}

}

EFI_STATUS set_desired_mode(gop) {
	EFI_STATUS status = gop->SetMode(gop, desired_mode);
	if (status == EFI_SUCCESS)
		video_output.cur_mode = desired_mode;
    return status;
}

void handle_err(EFI_STATUS status) {
    switch(status) {
        case EFI_SUCCESS:
            Print("Success\n");
            return;
        case EFI_UNSUPPORTED:
            Print("EFI_UNSUPPORTED Error\n");
            break;
        case EFI_NOT_FOUND:
            Print("EFI_NOT_FOUND Error\n");
            break;
        case EFI_BUFFER_TOO_SMALL:
            Print("EFI_BUFFER_TOO_SMALL Error\n");
            break;
        case EFI_INVALID_PARAMETER:
            Print("EFI_INVALID_PARAMETER Error\n");
            break;
        case EFI_ACCESS_DENIED:
            Print("EFI_ACCESS_DENIED Error\n");
            break;
        case EFI_ALREADY_STARTED:
            Print("EFI_ALREADY_STARTED Error\n");
            break;
        case EFI_OUT_OF_RESOURCES:
            Print("EFI_OUT_OF_RESOURCES Error\n");
            break;
        default:
            Print("Unexpected Error\n");
            break;
    }
    
    WaitForSingleEvent(SysTable->ConIn->WaitForKey, 0);
}
