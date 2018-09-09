    .intel_syntax noprefix
    .global load_idt
    .global store_idt

load_idt:
    lidt [rdi]
    ret

store_idt:
    sidt [rdi]
    ret
