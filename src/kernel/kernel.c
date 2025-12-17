/* Minimal freestanding kernel in C that writes to VGA text memory */
#include <types.h>
#include <io.h>
#include <vga.h>
#include <idt.h>
#include <timer.h>
#include <gdt.h>
#include <keyboard.h>
#include <multiboot.h>
#include <memory.h>




void kernel_main(struct multiboot_info *mbd, uint32_t magic) {
    // extern uint32_t stack_top;
    // __asm__ volatile("movl %0, %%esp" : : "r"(stack_top));
    
    initGdt();
    initIdt();
    pit_init(100);
    initKeyboard();

    uint32_t mod1 = *(uint32_t *)(mbd->mods_addr + 4); 
    uint32_t physicalAddress = (mod1 + 0xFFF) & ~0xFFF;

    initMemory(mbd->mem_upper * 1024, physicalAddress, mbd);
    // pic_remap();
    // __asm__ volatile("sti");
    // pit_init(100);
   
    // clear_screen();
    enable_cursor(0, 15);
    update_cursor();
    // clear_screen(); 
    
   
    puts("Welcome to my kernel");
    putc('\n');
    // putu32(cursor_col);
    

    // for (uint16_t i = 0; i < 20; i++) {
    //     puts("Line #");
    //     putu32(i);
    //     puts("\n");
    // }
    
    puts("\nDone! Kernel entering idle loop");
    // cursor_scroll();
    
    // uint8_t *buffer = cursor_row * 80 + cursor_col;
    // buffer[1] = 0xFFF;
   
     
//    unsigned int last = 0;
    /* Loop forever */
    for (;;) {
        // enable interupts
        __asm__("hlt");
        
    }
}