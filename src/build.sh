#!/bin/bash

x86_64-w64-mingw32-gcc -ffreestanding -I../gnu-efi/inc -I../gnu-efi/inc/x86_64 -I../gnu-efi/inc/protocol -c -o hello.o hello.c
x86_64-w64-mingw32-gcc -ffreestanding -I../gnu-efi/inc -I../gnu-efi/inc/x86_64 -I../gnu-efi/inc/protocol -c -o data.o data.c
x86_64-w64-mingw32-gcc -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 -e efi_main -o BOOTX64.EFI hello.o data.o -L../gnu-efi/x86_64/gnuefi -L../gnu-efi/x86_64/lib -lefi -lgnuefi -lgcc

dd if=/dev/zero of=fat.img bs=1k count=1440
mformat -i fat.img -f 1440 ::
mmd -i fat.img ::/EFI
mmd -i fat.img ::/EFI/BOOT
mcopy -i fat.img BOOTX64.EFI ::/EFI/BOOT

