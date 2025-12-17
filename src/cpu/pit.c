#include <types.h>
#include <pit.h>
#include <io.h>
#include <pic.h>
#include <idt.h>


extern void timer_stub();       // from isr.s
extern void timer_callback();   // your C handler

void pit_init(uint32_t frequency) {

    // Program PIT
    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);            // mode 3, LSB/MSB
    outb(0x40, divisor & 0xFF);  // LSB
    outb(0x40, (divisor >> 8));  // MSB

    // Unmask IRQ0 in PIC
    // pic_unmask_irq(0);
}
