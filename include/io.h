#ifndef IO_H
#define IO_H

#include <types.h>
struct InterruptRegisters{
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, csm, eflags, useresp, ss;
};

void outb(uint16_t port, uint8_t value);

uint8_t inb(uint16_t port);


void memset(void *dest, char val, uint32_t count);
void initKeyboard();

#endif
