SRCDIR := $(TOPDIR)/src

export AS := as
export CC := gcc-7
export LD := ld
OBJCOPY := objcopy
ARCH := x86_64

DIRSTEM := EFI/BOOT
export OBJDIR := $(TOPDIR)/obj/$(DIRSTEM)
BUILDDIR := $(TOPDIR)/build/$(DIRSTEM)
EFIDIR := $(TOPDIR)/gnu-efi

export INCDIR := -I $(SRCDIR) -I $(EFIDIR)/inc -I $(EFIDIR)/inc/$(ARCH) -I $(EFIDIR)/inc/protocol
export CPPFLAGS := -DCONFIG_$(ARCH) -DGNU_EFI_USE_MS_ABI -maccumulate-outgoing-args --std=c11 -D__KERNEL__ -I /usr/src/sys/build/include
export CFLAGS := -mno-red-zone -mno-avx -fPIC  -ggdb -Wall -Wextra -Werror -fshort-wchar -fno-strict-aliasing -fno-merge-all-constants -ffreestanding -fno-stack-protector -fno-stack-check
export ASMFLAGS := -g -Wall -k -no-pie
LDFLAGS := -nostdlib --warn-common --no-undefined --fatal-warnings --build-id=sha1 -shared -Bsymbolic -L $(EFIDIR)/$(ARCH)/lib -L $(EFIDIR)/$(ARCH)/gnuefi $(EFIDIR)/$(ARCH)/gnuefi/crt0-efi-$(ARCH).o

LIBGCC := $(shell $(CC) -print-libgcc-file-name)
LDLIBS := -lefi -lgnuefi $(LIBGCC) -T $(EFIDIR)/gnuefi/elf_$(ARCH)_efi.lds

TARGETS := BOOTX64.EFI BOOTX64.debug

SUBDIRS := boot cpu stdlib util
OBJECTS := kmain.o
LDOBJECTS := $(wildcard $(OBJDIR)/*.o)

all: $(TARGETS)

.PHONY: $(SUBDIRS)

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

%.so: $(SUBDIRS) $(OBJECTS) $(BUILDDIR)
	@echo "SUBDIRS: " $(SUBDIRS)
	cd $(OBJDIR); $(LD) $(LDFLAGS) $(LDOBJECTS) -o $(BUILDDIR)/$@ $(LDLIBS)

$(SUBDIRS):
	cd $@; $(MAKE)

$(OBJDIR):
	mkdir -p $@

$(BUILDDIR):
	mkdir -p $@

%.o: %.c $(OBJDIR)
	$(CC) $(INCDIR) $(CFLAGS) $(CPPFLAGS) -c $< -o $(OBJDIR)/$@

%.o: %.s $(OBJDIR)
	$(AS) $(ASMFLAGS) -c $< -o $(OBJDIR)/$@
