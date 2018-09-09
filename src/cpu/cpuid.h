#ifndef CPUID_H
#define CPUID_H

#include <efi.h>

extern void cpuid(UINTN rax_in, UINTN *rax_out, UINTN *rbx_out, UINTN *rcx_out, UINTN *rdx_out);

#endif
