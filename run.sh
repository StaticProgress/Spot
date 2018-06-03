#Universal Flags
flags=""

#If you use the 'debug' flag while running, qemu will wait for GDB
if [[ $# -ge 1 && $1 = "debug" ]]; then
    #Debug Flags
    flags="$flags -s" 
fi

if [[ $(uname -r | sed -n 's/.*\( *Microsoft *\).*/\1/p') = Microsoft ]]; then
	qemu-system-x86_64.exe -pflash bios.bin -hda fat:build -net none $flags
else
	qemu-system-x86_64 -enable-kvm -pflash bios.bin -hda fat:build -net none $flags
fi
