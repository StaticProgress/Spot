#This is a wrapper that will call our other interrupt methods.
#Idea taken from OSDev wiki: https://wiki.osdev.org/Interrupt_Service_Routines

.intel_syntax noprefix

.macro push_caller
    #Push all caller-save registers
    push rax
    push rcx
    push rdx
    push r8
    push r9
    push r10
    push r11
.endm

.macro pop_caller
    #Pop all caller-save registers
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rcx
    pop rax
.endm

#Idea of a ISR Macro taken from Aura:
#https://github.com/neonorb/aura/blob/1412cf87a8d541820c6e61283397db21b54ca623/src/kernel/idtasm.s
.macro ISR_NO_ERROR os_callback:req
.global isr_\os_callback
isr_\os_callback\():
    push rdi
    mov rdi, rsp #First argument of the os callback should be the interrupt_frame
    add rdi, 40
    push_caller
    call \os_callback
    pop_caller
    pop rdi
    iretq 
.endm

.macro ISR_ERROR os_callback
    push rdi
    mov rdi, rsp #Interrupt frame struct
    add rdi, 48
    push rsi
    mov rsi, rsp #Error Code argument
    add rsi, 8
    push_caller
    call \os_callback
    pop_caller
    pop rdi
    pop rsi
    iretq 
.endm

.global isr_unknown
isr_unknown:
    push_caller
    cld
    call unknown_print
    pop_caller
    iretq

.extern div_ex_callback
ISR_NO_ERROR div_ex_callback

