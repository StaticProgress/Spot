#include "gdt.h"
#include "mem_desc.h"
#include "print.h"

static gdt_ptr gdt_p;
static segment_desc gdt[3];

void setup_flat_gdt() {
    //Setup the pointer to our array of segment descriptors.
    gdt_p.base = (UINTN) gdt;
    gdt_p.limit = (UINT16) sizeof(gdt);

    //Setup the array of segment descriptors for a flat memory model (i.e. segmentation does nothing)
    setup_seg_desc(&gdt[0], 0x0, 0x0, 0x0, 0x0); //Null Descriptor
    setup_seg_desc(&gdt[1], 0x0, 0xFFFFFFFF, T_TYPE_CODE, LIM_FLAGS);
    setup_seg_desc(&gdt[2], 0x0, 0xFFFFFFFF, T_TYPE_DATA, LIM_FLAGS);

    //Now we have to load the gdt into the gdtr.
    load_gdt(&gdt_p);
    //TODO: Why is it crashing???
    //Is it because there is some sort of problem with the stack, check the stack before and after the function call to make sure that the return address is in the right spot.
    //
    //Now check where we stored it compared to what we loaded.
    printf("Loaded: %d\n", gdt_p.base);
    sgdt(&gdt_p);
    printf("Set to: %d\n", gdt_p.base);
    segment_desc *temp = (segment_desc*) gdt_p.base;
    for(int i = 0; i < 3; i++) {
        printf("GDT[%d] Starts at %d and goes for %d\n", i, temp[i].base_low, temp[i].limit);
    }

    while(1) {}
}

void sgdt(gdt_ptr *gdt_p) {
 //   __asm__ volatile("sgdt (%0)"::"p"(gdt_p):);
    (void) gdt_p;
}
