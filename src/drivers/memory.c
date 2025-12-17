#include <multiboot.h>
#include <memory.h>
#include <vga.h>
#include <io.h>

#define NUM_PAGE_DIRS 256
#define NUM_PAGE_FRAMES 0x100000000 / 0x1000 / 8


static uint32_t num_pages;
static uint64_t bit_map_size;
static uint8_t *physicalMemoryBitMap;
static uint32_t next_heap_vaddr;

static uint32_t totalMem; 


void pmm_init(uint32_t memLow, uint32_t memHigh, struct multiboot_info *mbd) {
    totalMem = 0;
    uint64_t max_addr = 0;

    // find all memory in the system
    for (int i = 0; i < mbd->mmap_length; i += sizeof(struct multiboot_mmap_entry)) {
        struct multiboot_mmap_entry *entry = (struct multiboot_mmap_entry*)(mbd->mmap_addr + i);

        // printf("StartLow: %x, StartHigh: %x, LengthLow: %x, Lengthhigh: %x, Type: %x\n", entries[i].addr_low, entries[i].addr_high, entries[i].len_low, entries[i].len_high, entries[i].type);
        printf("StartLow: %x, StartHigh: %x, LengthLow: %x, LengthHigh: %x, Type: %x\n", entry->addr_low, entry->addr_high, entry->len_low, entry->len_high, entry->type);
        

        uint64_t base_addr = ((uint64_t)entry->addr_high << 32) | entry->addr_low;
        uint64_t length    = ((uint64_t)entry->len_high  << 32) | entry->len_low;

        uint32_t end = base_addr + length;

        if (end > max_addr) {
            max_addr = end;
        }
    }

    num_pages = max_addr / 4096;
    bit_map_size = num_pages / 8;
    physicalMemoryBitMap[bit_map_size];
    memset(physicalMemoryBitMap, 0xFF, bit_map_size); // mark all as used

    //mark bitmap as free if region type == 1 (available)
    for (int i = 0; i < mbd->mmap_length; i += sizeof(struct multiboot_mmap_entry)) {
        struct multiboot_mmap_entry *entry = (struct multiboot_mmap_entry*)(mbd->mmap_addr + i);
        
        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint64_t base_addr = ((uint64_t)entry->addr_high << 32) | entry->addr_low;
            uint64_t length    = ((uint64_t)entry->len_high  << 32) | entry->len_low;

            for (uint32_t i = base_addr; i < length; i += 4096) {
                uint64_t page = i / 4096;
                physicalMemoryBitMap[page / 8] &= ~(1 << (page % 8)); // clear bit / mark as free
            }
            
        }
    }

    // mark kernel memory as used
    printf("Kernel end address %x\n", ((uint32_t)&_kernel_end - KERNEL_START));
    uint32_t kernel_end_physical = (uint32_t)&_kernel_end - KERNEL_START;
    uint32_t start_page = KERNEL_START / 4096;
    uint32_t end_page =  kernel_end_physical / 4096;

    for (uint32_t i = start_page; i < end_page; i += 4096) {
        uint32_t page = i / 4096;
        physicalMemoryBitMap[page / 8] |= 1 << (page % 8); // mark as used
    }


    // mark initial page dir as used
    uint32_t pd_addr_phys = (uint32_t)&initial_page_dir - KERNEL_START;
    uint32_t pd_end  = pd_addr_phys + sizeof(initial_page_dir);  // usually 4 KB

    for (uint32_t i = pd_addr_phys; i < pd_end; i += 4096) {
        uint32_t page = i / 4096;
        physicalMemoryBitMap[page / 8] |= (1 << (page % 8));
    }

    //mark page tables as sued memory
    uint32_t *pd = (uint32_t*)&initial_page_dir;

    for (int i = 0; i < 1024; i++) {
        uint32_t entry = pd[i];

        if (entry & 1) { // Present bit set
            uint32_t pt_phys = entry & 0xFFFFF000; // mask out flags
            uint32_t page = pt_phys / 4096;
            physicalMemoryBitMap[page / 8] |= (1 << (page % 8));
        }
    }

    //mark modules as used memory
    multiboot_module_t *mod = (multiboot_module_t*)mbd->mods_addr;
    for (uint32_t i = 0; i < mbd->mods_count; i++, mod++) {
        uint32_t start = mod->mod_start;
        uint32_t end   = mod->mod_end;
        for (uint32_t addr = start; addr < end; addr += 4096) {
            uint32_t page = addr / 4096;
            physicalMemoryBitMap[page / 8] |= (1 << (page % 8));  // mark used
        }
    }

}

uint32_t pmm_alloc_frame() {
    for (uint32_t i = 0; i < num_pages; i++) {
        uint32_t byte = i / 8;
        uint8_t bit = i % 8;

        if (!(physicalMemoryBitMap[byte] & (1 << bit))) {
            physicalMemoryBitMap[byte] |= 1 << bit; // set as used
            return i * PAGE_SIZE;
        }
    }

    return 0xFFFFFFFF; // invalid
}

void pmm_free_frame(uint32_t addr) {
    uint32_t page = addr / PAGE_SIZE;

    uint32_t byte = page / 8;
    uint8_t bit = page % 8;

    physicalMemoryBitMap[byte] &= ~(1 << bit); // set as free
}

uint32_t *get_page_table(uint32_t virt_addr, int create) {
    uint32_t pd_index = virt_addr >> 22;      // top 10 bits
    uint32_t pt_index = (virt_addr >> 12) & 0x3FF; // next 10 bits

    uint32_t pd_entry = initial_page_dir[pd_index];

    if (!(pd_entry & PAGE_FLAG_PRESENT)) {
        if (!create) return 0;

        // Allocate a new page table
        uint32_t pt_phys = pmm_alloc_frame();
        if (pt_phys == 0xFFFFFFFF) return 0; // out of memory

        // Clear new table
        uint32_t *pt_virt = (uint32_t *)(pt_phys + 0xC0000000); // map into kernel virtual space
        for (int i = 0; i < 1024; i++) pt_virt[i] = 0;

        // Update PD entry
        initial_page_dir[pd_index] = pt_phys | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
        pd_entry = initial_page_dir[pd_index];
    }

    uint32_t *pt = (uint32_t *)((pd_entry & 0xFFFFF000) + 0xC0000000);
    return pt;
}

void map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags) {
    uint32_t *pt = get_page_table(virt_addr, 1); // create PT if missing
    if (!pt) return; // out of memory

    uint32_t pt_index = (virt_addr >> 12) & 0x3FF;
    pt[pt_index] = (phys_addr & 0xFFFFF000) | (flags & 0xFFF);

    // Invalidate TLB for this virtual address
    __asm__ volatile("invlpg (%0)" ::"r"(virt_addr) : "memory");
}

void *kmalloc_page() {
    uint32_t phys = pmm_alloc_frame();
    if (phys == 0xFFFFFFFF) return 0;

    uint32_t virt = next_heap_vaddr;
    map_page(virt, phys, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);
    next_heap_vaddr += PAGE_SIZE;
    return (void *)virt;
}

void initMemory(uint32_t memHigh, uint32_t physicalAddressStart, struct multiboot_info *mbd) {
   
    printf("Upper Memory: %x\n", mbd->mem_upper);
    printf("Lower Memory: %x\n", mbd->mem_lower);
    printf("mmapAddr Memory: %x\n", mbd->mmap_addr);
    printf("Initial_Page_Dir: %x\n", initial_page_dir[0]);
    // initial_page_dir[0] = 0;
    // invalidate(0);
    initial_page_dir[1023] = ((uint32_t) initial_page_dir - KERNEL_START) | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
    
    invalidate(0xFFFFF000);

    pmm_init(physicalAddressStart, memHigh, mbd);
    next_heap_vaddr = ((uint32_t)&_kernel_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    test_kmalloc();
    
}

void test_kmalloc() {
    void *p = kmalloc_page(PAGE_SIZE);  // allocate one page
    if (!p) {
        puts("kmalloc failed!\n");
        return;
    }

    // Write some data
    char *c = (char*)p;
    for (int i = 0; i < PAGE_SIZE; i++) {
        c[i] = (char)(i & 0xFF);
    }

    // Read it back
    for (int i = 0; i < PAGE_SIZE; i++) {
        if (c[i] != (char)(i & 0xFF)) {
            puts("Memory corruption!\n");
            return;
        }
    }

    puts("kmalloc page test OK\n");
}
void invalidate(uint32_t vaddr) {
    asm volatile("invlpg %0" :: "m"(vaddr));
}