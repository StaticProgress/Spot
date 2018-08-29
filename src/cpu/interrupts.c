#include <efi.h>
#include "interrupts.h"
#include "stdlib/print.h"

void interrupts_off() {
    __asm__("cli");
}

void interrupts_on() {
    __asm__("sti");
}

// --- AMD64 Exception Interrupt Handlers ---
//TODO: Change these from prints to signals at a later date, or atleast unbuffered printing.

void div_ex_callback(interrupt_frame *frame) {
    #ifdef INT_LOGGING
    printf("Division by Zero Exception at RIP: %d\n", frame->rip);
    #endif
}
