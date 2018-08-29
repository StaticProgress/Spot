#include "mem_desc.h"

/* This is a file for writing x64 Descriptors used
 * in places like the GDT and the IDT.
 */

void setup_seg_desc(segment_desc *seg_d, UINT32 base, UINT32 lim, UINT8 type, UINT8 lim_flags) {
    seg_d->limit = (UINT16) lim; 
    seg_d->base_low = (UINT16) base;
    seg_d->base_middle = (UINT8)((base << 8) >> 24);
    seg_d->type = type;
    seg_d->lim_flags = lim_flags | ((lim >> 16) & 0xf);
    seg_d->base_high = (UINT8) base >> 24;
}

void setup_gate_desc(gate_desc *gate_d, UINT64 base, UINT16 cs_selector, UINT8 type) {
    gate_d->base_low = (UINT16) base;
    gate_d->selector = cs_selector;
    gate_d->zero_low = 0;
    gate_d->type = type;
    gate_d->base_middle = (UINT16)(base >> 16);
    gate_d->base_high = (UINT32)(base >> 32);
    gate_d->zero_high = 0;
}

