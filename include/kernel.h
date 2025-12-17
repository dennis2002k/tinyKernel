#ifndef KERNEL_H
#define KERNEL_H
#include <multiboot.h>
void kernel_main(struct multiboot_info* mbd, uint32_t magic);


#endif
