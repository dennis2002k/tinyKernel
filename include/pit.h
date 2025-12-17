#ifndef PIT_H
#define PIT_H

#include <types.h>
// #include <isr.h>  // If using registers_t struct in callback

// Initialize the PIT to a given frequency (Hz)
void pit_init(uint32_t frequency);


#endif