#include <efi.h>

#include "kmain.h"
#include "memalloc.h"
#include "kprint.h"
#include "graphics.h"

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
}
