#ifdef BOOT_H
#define BOOT_H

#include <efi.h>
#include <efilib.h>

/* EFI BOOT FUNCTIONS */
/* These functions are to be used only before we exit boot services */

//Graphics Output related functions
void setup_gop(EFI_HANDLE); //Calls all the other gop related functions.
EFI_STATUS locate_gop(UINTN, EFI_HANDLE);
EFI_STATUS open_gop(EFI_HANDLE*, EFI_GRAPHICS_OUTPUT_PROTOCOL*, EFI_HANDLE);
void setup_video_output(EFI_GRAPHICS_OUTPUT_PROTOCOL*); 
EFI_STATUS set_desired_mode(EFI_GRAPHICS_OUTPUT_PROTOCOL*);

//Memory Map related functions
EFI_STATUS get_mem_map(UINTN *map_key, EFI_MEMORY_DESCRIPTOR *mem_map);

//Simple Error Message printing function
//Will print a small error description if there is one or success if not.
//Also will pause and wait for a keystroke before continueing
void handle_err(EFI_STATUS status);

#endif
