info files
file
add-symbol-file build/EFI/BOOT/BOOTX64.debug 0x068F5000 -s .data 0x068D000
set architecture i386:x86-64:intel
target remote :1234
