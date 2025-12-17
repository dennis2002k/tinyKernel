#include <types.h>
#include <io.h>
#include <vga.h>
#include <idt.h>
#include <time.h>

uint64_t ticks;

void timer_callback(struct InterruptRegisters *r) {
    
    ticks++; 
    // puts("Tick");
   
}

void pit_init(uint32_t frequency) {
    ticks = 0;
    irq_install_handler(0, &timer_callback);

    // Program PIT
    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);            // mode 3, LSB/MSB
    outb(0x40, (uint8_t)(divisor & 0xFF));  // LSB
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));  // MSB

    // Unmask IRQ0 in PIC
    // pic_unmask_irq(0);
}