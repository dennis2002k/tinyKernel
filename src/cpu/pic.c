#include <types.h>
#include <io.h>
#include <pic.h>
#include <vga.h>

volatile int irq_test_flag = 0;
volatile int irq_test_flag2 = 0;

void pic_remap() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);  // Master PIC offset = 0x20 (IRQ0-7)
    outb(0xA1, 0x28);  // Slave PIC offset = 0x28 (IRQ8-15)

    outb(0x21, 0x04);  // Tell Master that there is a slave at IRQ2
    outb(0xA1, 0x02);  // Tell Slave its cascade identity

    outb(0x21, 0x01);  // 8086 mode
    outb(0xA1, 0x01);

    // Mask all IRQs initially
    outb(0x21, 0xFE);
    outb(0xA1, 0xFF);
}

void pic_unmask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8)
        port = 0x21;
    else {
        port = 0xA1;
        irq -= 8;
    }

    value = inb(port) & ~(1 << irq);
    irq_test_flag = value;
    irq_test_flag2 = port;
    outb(port, value);
}
