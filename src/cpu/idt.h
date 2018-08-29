#ifndef IDT_H
#define IDT_H

#include "mem_desc.h"

/* Loads the IDT with basic info printing exception handlers.
 * These are not meant to be final product exception / interrupt handlers.
 * CAUTION: This method assumes that interrupts are disabled and should be called right before the gdt is changed.
 */
void setup_basic_idt(); 
extern void load_idt(desc_ptr *idt_p);
extern void store_idt(desc_ptr *idt_p);

#endif
