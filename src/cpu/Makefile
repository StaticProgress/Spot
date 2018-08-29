OBJECTS := apic.o cpuid.o gdt_util.o gdt.o idt_util.o idt.o interrupts.o isr_wrap.o mem_desc.o

all: $(OBJECTS)

%.o: %.c $(OBJDIR)
	$(CC) $(INCDIR) $(CFLAGS) $(CPPFLAGS) -c $< -o $(OBJDIR)/$@

%.o: %.s $(OBJDIR)
	$(AS) $(ASMFLAGS) -c $< -o $(OBJDIR)/$@
