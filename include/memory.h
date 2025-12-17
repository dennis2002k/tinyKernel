// #include <multiboot.h>
void initMemory(uint32_t memHigh, uint32_t physicalAddressStart, struct multiboot_info *mbd);
void invalidate(uint32_t vaddr);
void pmm_init(uint32_t memLow, uint32_t memHigh, struct multiboot_info *mbd);
void test_kmalloc();

extern uint32_t initial_page_dir[1024];
extern uint32_t _kernel_end;
#define KERNEL_START 0xC0000000
#define KERNEL_MALLOC 0xD0000000
#define KERNEL_START_PHYSICAL 0x100000
#define PAGE_FLAG_PRESENT (1 << 0)
#define PAGE_FLAG_WRITE (1 << 1)
#define NUM_PAGE_TABLES 5
#define PAGE_SIZE 4096

#define CEIL_DIV(a, b) ((a + b - 1) / b)

