#ifndef MEM_DESC_H
#define MEM_DESC_H

#include <efi.h>

// When Wikipedia comes in clutch with the resources...
// https://en.wikipedia.org/wiki/Segment_descriptor#x86-64
// Combined with the glorious OSDev wiki
// https://wiki.osdev.org/Descriptors
// I don't think this changes format because we are in x64 mode.
// https://www.intel.com/content/dam/support/us/en/documents/processors/pentium4/sb/25366821.pdf
typedef struct {
    UINT16 limit;
    UINT16 base_low;
    UINT8 base_middle;
    UINT8 type;
    UINT8 lim_flags;
    UINT8 base_high;
} __attribute__((packed)) segment_desc;

typedef struct {
    UINT16 base_low;
    UINT16 selector;
    UINT8 zero_low;
    UINT8 type;
    UINT16 base_middle;
    UINT32 base_high;
    UINT32 zero_high;
} __attribute__((packed)) gate_desc;

typedef struct {
    UINT16 limit;
    UINTN base;
} __attribute__((packed)) desc_ptr;

//To help me later the first letter of these flags signals if it should be 
//or'd in the type or lim_flags field.
#define T_TYPE_CODE 0b10011010
#define T_TYPE_DATA 0b10010010
#define LIM_FLAGS 0b10100000
#define IDT_TRAP 0x8F
#define IDT_INTERRUPT 0x8E

void setup_seg_desc(segment_desc *seg_d, UINT32 base, UINT32 lim, UINT8 type, UINT8 lim_flags);
void setup_gate_desc(gate_desc *gate_d, UINT64 base, UINT16 cs_selector, UINT8 type);

#endif
