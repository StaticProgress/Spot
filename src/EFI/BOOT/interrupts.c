void interrupts_off() {
    __asm__("cli");
}

void interrupts_on() {
    __asm__("sti");
}
