#ifndef TIMER_H
#define TIMER_H

#include <types.h>
#include <io.h>


// void timer_stub();
void timer_callback(struct InterruptRegisters *r);
void pit_init(uint32_t frequency);

#endif
