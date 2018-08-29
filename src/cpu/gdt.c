#include "gdt.h"
#include "mem_desc.h"
#include "stdlib/print.h"
#include "interrupts.h"

static desc_ptr gdt_p;
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
}
