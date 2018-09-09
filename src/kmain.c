#include <efi.h>

#include "kmain.h"
#include "stdlib/kprint.h"
#include "stdlib/print.h"
#include "stdlib/graphics.h"
#include "cpu/interrupts.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "cpu/cpuid.h"
#include "stdlib/mem/ppm.h"

static void draw_logo() {
    // get frame buffer base and draw a square. note that qemu's 1024x768 mode is 24 bits (found in PixelBitMask)
	UINTN x = 1024 / 2, y = 768 / 2, w = 100, h = 50;
    draw_rect(x - (w / 2), y - (h / 2), w, h, 0xFFFFFF);

	// function to print string at specified x and y coordinates
    kprint(x - 45, y - 14, 0xEF7223, "Welcome to\nSpot OS!\n");
}

void kmain(EFI_MEMORY_DESCRIPTOR *mem_map, UINTN map_size, UINTN desc_size) {
    clear_screen();
    draw_logo();

    UINTN page_num = 0;
    (void) desc_size;
    for(UINTN i = 0; i < map_size; i += desc_size) {
        EFI_MEMORY_DESCRIPTOR *curr_desc = (EFI_MEMORY_DESCRIPTOR*)((char*)mem_map + i);
        page_num += curr_desc->NumberOfPages;
    }

    printf("Current Memory Size in Pages: %d\n", page_num);

    printf("Attempting to setup GDT / IDT ... ");

    interrupts_off();
    setup_flat_gdt();
    setup_basic_idt();
    interrupts_on();

    printf("Done!\n");

    //Setup malloc and physical page management.
    setup_ppm(mem_map, map_size, desc_size);

    //Check to see if the APIC is supported.
    UINTN *edx = 0;
    UINTN *trash = 0;
    cpuid(0x01, trash, trash, trash, edx);
    UINTN pic_mask = 1 << 9;
    if(*edx & pic_mask)
        printf("APIC supported!\n");

    //So we don't get UEFI restarting and get confused for a week again...
    while(1){}
}
