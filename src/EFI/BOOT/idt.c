#include "idt.h"
#include "mem_desc.h"
#include "print.h"
#include "string.h"

static desc_ptr idt_p;
static gate_desc idt_vec[32];

extern void isr_wrapper();

void simple_print() {
    printf("Exception!\n");
}

void setup_basic_idt() {
    idt_p.base = (UINTN) idt_vec;
    idt_p.limit = sizeof(idt_vec);
    
    //Setup the idt_vec with a basic printing function.
    //Set the reserved interrupts to 0. (9, 15, 20-29, 31)
    spot_memset(&idt_vec[9], 0x0, sizeof(gate_desc));
    spot_memset(&idt_vec[15], 0x0, sizeof(gate_desc));
    spot_memset(&idt_vec[31], 0x0, sizeof(gate_desc));

    for(int i = 20; i < 30; i++) {
        spot_memset(&idt_vec[i], 0x0, sizeof(gate_desc));
    }
    
    //Set all the traps to the basic printing function (1 - 4).
    for(int i  = 1; i < 5; i++) {
        setup_gate_desc(&idt_vec[i], (UINT64) isr_wrapper, 0x08, IDT_TRAP);
    }

    //Set all the faults and aborts to interrupt gates with basic printing function.
    //(0, 5-8, 10-14, 16-19, 30)
    setup_gate_desc(&idt_vec[0], (UINT64) isr_wrapper, 0x08, IDT_INTERRUPT);
    setup_gate_desc(&idt_vec[30], (UINT64) isr_wrapper, 0x08, IDT_INTERRUPT);
    
    for(int i = 5; i < 9; i++) {
        setup_gate_desc(&idt_vec[i], (UINT64) isr_wrapper, 0x08, IDT_INTERRUPT); 
    }
    
    for(int i = 10; i < 15; i++) {
        setup_gate_desc(&idt_vec[i], (UINT64) isr_wrapper, 0x08, IDT_INTERRUPT); 
    }

    for(int i  = 16; i < 19; i++) {
        setup_gate_desc(&idt_vec[i], (UINT64) isr_wrapper, 0x08, IDT_INTERRUPT);
    }    

    load_idt(&idt_p);
}
