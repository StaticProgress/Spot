#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <efi.h>

//Print the interrupt that is processed.
#define INT_LOGGING

typedef struct {
    UINTN ss;
    UINTN rsp;
    UINTN rflags;
    UINTN cs;
    UINTN rip;
} __attribute__((packed)) interrupt_frame;

void interrupts_on();
void interrupts_off();

extern void isr_div_ex_callback();
void div_ex_callback(interrupt_frame *frame);

#endif
