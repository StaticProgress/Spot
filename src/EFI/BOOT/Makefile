SRCDIR := $(TOPDIR)/src

AS := as
CC := gcc-7
LD := ld
OBJCOPY := objcopy
ARCH := x86_64

DIRSTEM := EFI/BOOT
OBJDIR := $(TOPDIR)/obj/$(DIRSTEM)
BUILDDIR := $(TOPDIR)/build/$(DIRSTEM)
EFIDIR := $(TOPDIR)/gnu-efi

INCDIR := -I $(SRCDIR) -I $(EFIDIR)/inc -I $(EFIDIR)/inc/$(ARCH) -I $(EFIDIR)/inc/protocol

CPPFLAGS := -DCONFIG_$(ARCH) -DGNU_EFI_USE_MS_ABI -maccumulate-outgoing-args --std=c11 -D__KERNEL__ -I /usr/src/sys/build/include

CFLAGS := -mno-red-zone -mno-avx -fPIC  -ggdb -Wall -Wextra -Werror -fshort-wchar -fno-strict-aliasing -fno-merge-all-constants -ffreestanding -fno-stack-protector -fno-stack-check

ASMFLAGS := -g -Wall -k -no-pie

LDFLAGS := -nostdlib --warn-common --no-undefined --fatal-warnings --build-id=sha1 -shared -Bsymbolic -L $(EFIDIR)/$(ARCH)/lib -L $(EFIDIR)/$(ARCH)/gnuefi $(EFIDIR)/$(ARCH)/gnuefi/crt0-efi-$(ARCH).o

LIBGCC := $(shell $(CC) -print-libgcc-file-name)
LDLIBS := -lefi -lgnuefi $(LIBGCC) -T $(EFIDIR)/gnuefi/elf_$(ARCH)_efi.lds

TARGETS := BOOTX64.EFI BOOTX64.debug

OBJECTS := BOOTX64.o kprint.o graphics.o string.o print.o util.o mem_desc.o gdt.o kmain.o gdt_util.o idt_util.o idt.o interrupts.o isr_wrap.o

all: $(TARGETS)

%.EFI : %.so
	cd $(BUILDDIR); \
	$(OBJCOPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel \
		       -j .rela -j .rel.* -j .rela.* -j .rel* -j .rela*  -j .reloc \
			   --target efi-app-$(ARCH) $< $@;

%.debug : %.so
	cd $(BUILDDIR); \
	$(OBJCOPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel \
		       -j .rela -j .rel.* -j .rela.* -j .rel* -j .rela*  -j .reloc \
			   -j .debug_info -j .debug_abbrev -j .debug_loc -j .debug_aranges \
			   -j .debug_line -j .debug_macinfo -j .debug_str -j .debug* \
			   --target efi-app-$(ARCH) $< $@; \
	rm -rf $<

%.so: $(OBJECTS) $(BUILDDIR)
	cd $(OBJDIR); \
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(BUILDDIR)/$@ $(LDLIBS)

%.o: %.c $(OBJDIR)
	$(CC) $(INCDIR) $(CFLAGS) $(CPPFLAGS) -c $< -o $(OBJDIR)/$@

%.o: %.s $(OBJDIR)
	$(AS) $(ASMFLAGS) -c $< -o $(OBJDIR)/$@

$(OBJDIR):
	mkdir -p $@

$(BUILDDIR):
	mkdir -p $@
