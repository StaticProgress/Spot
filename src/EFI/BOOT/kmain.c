#include <efi.h>

#include "kmain.h"
#include "kprint.h"
#include "print.h"
#include "graphics.h"
#include "interrupts.h"
#include "gdt.h"
#include "idt.h"

static void draw_logo() {
    // get frame buffer base and draw a square. note that qemu's 1024x768 mode is 24 bits (found in PixelBitMask)
	UINTN x = 1024 / 2, y = 768 / 2, w = 100, h = 50;
    draw_rect(x - (w / 2), y - (h / 2), w, h, 0xFFFFFF);

	// function to print string at specified x and y coordinates
    kprint(x - 45, y - 14, 0xEF7223, "Welcome to\nSpot OS!\n");    
}

void kmain() {
    clear_screen();
    draw_logo();
    
    printf("Attempting to setup GDT...\n");
   
    interrupts_off();
    setup_flat_gdt();
    setup_basic_idt();
    interrupts_on();

    printf("Attempting to Throw an exception!\n");
    int j = 0;
    int i = 9 / j;
    (void) i;
    while(1) {}
}
