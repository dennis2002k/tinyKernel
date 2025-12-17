#ifndef PIC_H
#define PIC_H

#include <types.h>

// Remap PIC interrupts to avoid conflicts with CPU exceptions
void pic_remap();

// Send End of Interrupt (EOI) to PIC
// void pic_send_eoi(uint8_t irq);

#endif
