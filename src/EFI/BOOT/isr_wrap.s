#This is a wrapper that will call our other interrupt methods.
#Idea taken from OSDev wiki: https://wiki.osdev.org/Interrupt_Service_Routines

    .intel_syntax noprefix
    .global isr_wrapper

isr_wrapper:
    #Push all caller-save registers
    push rax
    push rcx
    push rdx
    push r8
    push r9
    push r10
    push r11
    cld
    call simple_print
    #Pop all caller-save registers
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rcx
    pop rax
    iretq
