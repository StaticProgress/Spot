if [[ $(uname -r | sed -n 's/.*\( *Microsoft *\).*/\1/p') = Microsoft ]]; then
	qemu-system-x86_64.exe -pflash bios.bin -hda fat:build -net none
else
	qemu-system-x86_64 -enable-kvm -pflash bios.bin -hda fat:build -net none
fi