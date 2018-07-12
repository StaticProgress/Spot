#ifndef GDT_H
#define GDT_H

#include <efi.h>
#include "mem_desc.h"

/* x64 Processor architecture is supposed to use a flat memory model for the gdt because 
 * 64 bit is not supposed to use segmentation. This means that the base is always 0 and the 
 * limit is always the max it can go, meaning that no checking is effectively done. 
 * CAUTION: Interrupts should be turned off before calling this method. 
 */
void setup_flat_gdt();
extern void load_gdt(desc_ptr *gdtp);
extern void store_gdt(desc_ptr *gdtp);

#endif
