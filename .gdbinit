info files
file
add-symbol-file build/EFI/BOOT/BOOTX64.debug 0x068F6000 -s .data 0x068FE000
set architecture i386:x86-64:intel
target remote :1234
