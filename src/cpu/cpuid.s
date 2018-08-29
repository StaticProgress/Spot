.intel_syntax noprefix
.global cpuid

#Four Arguments.
#RDI: rax_in
#RSI: *rax_out
#RDX: *rbx_out
#RCX: *rcx_out
#r8: *rdx_out
cpuid:
    push rbx
    mov rax, rdi
    mov r9, rdx
    mov r10, rcx #Save the pointer because we are about to clobber rd/cx
    cpuid #Places results in rax, rbx, rcx, rdx
    mov [rsi], rax
    mov [r9], rbx
    mov [r10], rcx
    mov [r8], rdx
    pop rbx
    ret
